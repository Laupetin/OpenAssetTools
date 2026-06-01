#include "XAnimLoaderIW3.h"

#include "Utils/Alignment.h"
#include "Utils/Logging/Log.h"
#include "Utils/StreamUtils.h"
#include "XAnim/XAnimCommon.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <format>
#include <limits>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

using namespace IW3;

namespace
{
    constexpr uint16_t RAW_VERSION = 17;

    constexpr uint8_t FLAG_LOOPED = 1u;
    constexpr uint8_t FLAG_DELTA = 2u;

    // The linker decodes raw trans size[] with these exact float literals.
    // They correspond to 1.0f / 255.0f and 1.0f / 65535.0f, but we keep the
    // decompiled values to preserve binary-stable round trips.
    constexpr auto HALF_TRANS_SIZE_SCALE = 0.003921568859368563f;
    constexpr auto FULL_TRANS_SIZE_SCALE = 0.00001525902189314365f;
    enum class QuatType : uint8_t
    {
        NO_QUAT = 0,
        HALF_QUAT = 1,
        FULL_QUAT = 2,
        HALF_QUAT_NO_SIZE = 3,
        FULL_QUAT_NO_SIZE = 4,
    };

    enum class TransType : uint8_t
    {
        SMALL_TRANS = 5,
        FULL_TRANS = 6,
        TRANS_NO_SIZE = 7,
        NO_TRANS = 8,
    };

    struct QuatTrack
    {
        QuatType type = QuatType::NO_QUAT;
        std::vector<uint16_t> indices;
        std::vector<int16_t> values;
    };

    struct TransTrack
    {
        TransType type = TransType::NO_TRANS;
        std::vector<uint16_t> indices;
        std::array<float, 3> mins{};
        std::array<float, 3> size{};
        std::vector<uint8_t> byteFrames;
        std::vector<uint16_t> shortFrames;
        std::array<float, 3> constant{};
    };

    struct BoneTrack
    {
        std::string name;
        QuatTrack quat;
        TransTrack trans;
    };

    struct FlatDataWriteCursor
    {
        std::vector<uint8_t> dataByte;
        std::vector<int16_t> dataShort;
        std::vector<int32_t> dataInt;
        std::vector<uint8_t> randomDataByte;
        std::vector<int16_t> randomDataShort;
        std::vector<uint16_t> indices;
    };

    void PrintError(const XAnimParts& parts, const std::string& message)
    {
        con::error("Cannot load xanim \"{}\": {}", parts.name, message);
    }

    [[nodiscard]] bool UseByteIndices(const XAnimParts& parts)
    {
        return parts.numframes < 256;
    }

    [[nodiscard]] int FloatBitsToInt(const float value)
    {
        union
        {
            int i;
            float f;
        };

        f = value;
        return i;
    }

    void WriteFloat3(FlatDataWriteCursor& writeCursor, const std::array<float, 3>& value)
    {
        for (const float f : value)
            writeCursor.dataInt.emplace_back(FloatBitsToInt(f));
    }

    [[nodiscard]] float DecodeRawTransSize(const float value, const bool smallTrans)
    {
        const auto scale = smallTrans ? HALF_TRANS_SIZE_SCALE : FULL_TRANS_SIZE_SCALE;
        return value * scale;
    }

    void ConsumeQuat(std::istream& stream, XQuat& quat)
    {
        quat.value[0] = stream::ReadValue<int16_t>(stream);
        quat.value[1] = stream::ReadValue<int16_t>(stream);
        quat.value[2] = stream::ReadValue<int16_t>(stream);

        int32_t temp = 0x3FFF0001 - (quat.value[0] * quat.value[0] + quat.value[1] * quat.value[1] + quat.value[2] * quat.value[2]);
        if (temp <= 0)
            temp = 0;
        else
            temp = static_cast<int32_t>(std::floor(std::sqrt(static_cast<float>(temp)) + 0.5f));

        assert(temp >= std::numeric_limits<int16_t>::min() && temp <= std::numeric_limits<int16_t>::max());
        quat.value[3] = static_cast<int16_t>(temp);
    }

    void ConsumeQuat2(std::istream& stream, XQuat2& quat2)
    {
        quat2.value[0] = stream::ReadValue<int16_t>(stream);

        int32_t temp = 0x3FFF0001 - quat2.value[0] * quat2.value[0];
        if (temp <= 0)
            temp = 0;
        else
            temp = static_cast<int32_t>(floor(std::sqrt(static_cast<float>(temp)) + 0.5f));

        assert(temp >= std::numeric_limits<int16_t>::min() && temp <= std::numeric_limits<int16_t>::max());
        quat2.value[1] = static_cast<int16_t>(temp);
    }

    void FlipQuat(XQuat& quat)
    {
        quat.value[0] = static_cast<int16_t>(-quat.value[0]);
        quat.value[1] = static_cast<int16_t>(-quat.value[1]);
        quat.value[2] = static_cast<int16_t>(-quat.value[2]);
        quat.value[3] = static_cast<int16_t>(-quat.value[3]);
    }

    void FlipQuat2(XQuat2& quat)
    {
        quat.value[0] = static_cast<int16_t>(-quat.value[0]);
        quat.value[1] = static_cast<int16_t>(-quat.value[1]);
    }

    template<typename T>
    void LoadIndicesIfNeeded(std::istream& stream, T& indices, const uint16_t numIndices, const bool useByteIndices, const uint16_t numLoopFrames)
    {
        if (useByteIndices)
        {
            // The raw format omits indices when a track covers every loop frame in order.
            if (numIndices >= numLoopFrames)
                std::iota(&indices._1[0], &indices._1[numIndices], 0);
            else
                stream::Read(stream, indices._1, numIndices * sizeof(uint8_t));
        }
        else
        {
            // The raw format omits indices when a track covers every loop frame in order.
            if (numIndices >= numLoopFrames)
                std::iota(&indices._2[0], &indices._2[numIndices], 0);
            else
                stream::Read(stream, indices._2, numIndices * sizeof(uint16_t));
        }
    }

    void LoadIndicesIfNeeded(
        std::istream& stream, std::vector<uint16_t>& indices, const uint16_t numIndices, const bool useByteIndices, const uint16_t numLoopFrames)
    {
        // The raw format omits indices when a track covers every loop frame in order.
        if (numIndices >= numLoopFrames)
        {
            indices.resize(numIndices);
            std::ranges::iota(indices, 0);
        }
        else if (useByteIndices)
        {
            indices.reserve(numIndices);
            for (auto i = 0u; i < numIndices; i++)
                indices.emplace_back(stream::ReadValue<uint8_t>(stream));
        }
        else
        {
            indices.resize(numIndices);
            stream::Read(stream, indices.data(), numIndices * sizeof(uint16_t));
        }
    }

    void ReadTransTrack(std::istream& stream, TransTrack& transTrack, const uint16_t numLoopFrames, const bool useByteIndices)
    {
        const auto numTransIndices = stream::ReadValue<uint16_t>(stream);
        if (numTransIndices == 0)
        {
            transTrack.type = TransType::NO_TRANS;
            return;
        }

        if (numTransIndices == 1)
        {
            transTrack.type = TransType::TRANS_NO_SIZE;
            for (auto& value : transTrack.constant)
                value = stream::ReadValue<float>(stream);
            return;
        }

        LoadIndicesIfNeeded(stream, transTrack.indices, numTransIndices, useByteIndices, numLoopFrames);

        const auto smallTrans = stream::ReadValue<bool>(stream);
        transTrack.type = smallTrans ? TransType::SMALL_TRANS : TransType::FULL_TRANS;

        for (auto& value : transTrack.mins)
            value = stream::ReadValue<float>(stream);
        for (auto& value : transTrack.size)
            value = DecodeRawTransSize(stream::ReadValue<float>(stream), smallTrans);

        if (smallTrans)
        {
            transTrack.byteFrames.resize(numTransIndices * 3);
            stream::Read(stream, transTrack.byteFrames.data(), numTransIndices * sizeof(uint8_t) * 3);
        }
        else
        {
            transTrack.shortFrames.resize(numTransIndices * 3);
            stream::Read(stream, transTrack.shortFrames.data(), numTransIndices * sizeof(uint16_t) * 3);
        }
    }

    void ReadQuatTrack(
        std::istream& stream, QuatTrack& quatTrack, const uint16_t numLoopFrames, const bool useByteIndices, const bool flipQuat, const bool halfQuat)
    {
        const auto numQuatIndices = stream::ReadValue<uint16_t>(stream);
        if (numQuatIndices == 0)
        {
            assert(halfQuat);
            quatTrack.type = QuatType::NO_QUAT;
            return;
        }

        if (numQuatIndices == 1)
        {
            quatTrack.type = halfQuat ? QuatType::HALF_QUAT_NO_SIZE : QuatType::FULL_QUAT_NO_SIZE;
            if (halfQuat)
            {
                XQuat2 quat2;
                ConsumeQuat2(stream, quat2);
                if (flipQuat)
                    FlipQuat2(quat2);

                quatTrack.values.reserve(2);
                quatTrack.values.emplace_back(quat2.value[0]);
                quatTrack.values.emplace_back(quat2.value[1]);
            }
            else
            {
                XQuat quat;
                ConsumeQuat(stream, quat);
                if (flipQuat)
                    FlipQuat(quat);

                quatTrack.values.reserve(4);
                quatTrack.values.emplace_back(quat.value[0]);
                quatTrack.values.emplace_back(quat.value[1]);
                quatTrack.values.emplace_back(quat.value[2]);
                quatTrack.values.emplace_back(quat.value[3]);
            }

            return;
        }

        LoadIndicesIfNeeded(stream, quatTrack.indices, numQuatIndices, useByteIndices, numLoopFrames);

        if (halfQuat)
        {
            quatTrack.type = QuatType::HALF_QUAT;
            quatTrack.values.resize(numQuatIndices * 2);
            auto* quats2 = reinterpret_cast<XQuat2*>(quatTrack.values.data());
            for (auto quatIndexNum = 0u; quatIndexNum < numQuatIndices; quatIndexNum++)
            {
                auto& curFrame = quats2[quatIndexNum];
                ConsumeQuat2(stream, curFrame);

                if (quatIndexNum > 0)
                {
                    const auto& prevFrame = quats2[quatIndexNum - 1];
                    if (prevFrame.value[0] * curFrame.value[0] + prevFrame.value[1] * curFrame.value[1] < 0)
                        FlipQuat2(curFrame);
                }
                else if (flipQuat)
                    FlipQuat2(curFrame);
            }
        }
        else
        {
            quatTrack.type = QuatType::FULL_QUAT;
            quatTrack.values.resize(numQuatIndices * 4);
            auto* quats = reinterpret_cast<XQuat*>(quatTrack.values.data());
            for (auto quatIndexNum = 0u; quatIndexNum < numQuatIndices; quatIndexNum++)
            {
                auto& curFrame = quats[quatIndexNum];
                ConsumeQuat(stream, curFrame);

                if (quatIndexNum > 0)
                {
                    const auto& prevFrame = quats[quatIndexNum - 1];
                    const auto dot = prevFrame.value[0] * curFrame.value[0] + prevFrame.value[1] * curFrame.value[1] + prevFrame.value[2] * curFrame.value[2]
                                     + prevFrame.value[3] * curFrame.value[3];
                    if (dot < 0)
                        FlipQuat(curFrame);
                }
                else if (flipQuat)
                    FlipQuat(curFrame);
            }
        }
    }

    void ApplyWriteCursorToParts(XAnimParts& parts, const FlatDataWriteCursor& writeCursor, MemoryManager& memory)
    {
        if (!writeCursor.dataByte.empty())
        {
            parts.dataByteCount = static_cast<uint16_t>(writeCursor.dataByte.size());
            parts.dataByte = memory.Alloc<uint8_t>(parts.dataByteCount);
            std::memcpy(parts.dataByte, writeCursor.dataByte.data(), parts.dataByteCount * sizeof(uint8_t));
        }

        if (!writeCursor.dataShort.empty())
        {
            parts.dataShortCount = static_cast<uint16_t>(writeCursor.dataShort.size());
            parts.dataShort = memory.Alloc<int16_t>(parts.dataShortCount);
            std::memcpy(parts.dataShort, writeCursor.dataShort.data(), parts.dataShortCount * sizeof(int16_t));
        }

        if (!writeCursor.dataInt.empty())
        {
            parts.dataIntCount = static_cast<uint16_t>(writeCursor.dataInt.size());
            parts.dataInt = memory.Alloc<int32_t>(parts.dataIntCount);
            std::memcpy(parts.dataInt, writeCursor.dataInt.data(), parts.dataIntCount * sizeof(int32_t));
        }

        if (!writeCursor.randomDataByte.empty())
        {
            parts.randomDataByteCount = static_cast<uint16_t>(writeCursor.randomDataByte.size());
            parts.randomDataByte = memory.Alloc<uint8_t>(parts.randomDataByteCount);
            std::memcpy(parts.randomDataByte, writeCursor.randomDataByte.data(), parts.randomDataByteCount * sizeof(uint8_t));
        }

        if (!writeCursor.randomDataShort.empty())
        {
            parts.randomDataShortCount = static_cast<unsigned int>(writeCursor.randomDataShort.size());
            parts.randomDataShort = memory.Alloc<int16_t>(parts.randomDataShortCount);
            std::memcpy(parts.randomDataShort, writeCursor.randomDataShort.data(), parts.randomDataShortCount * sizeof(int16_t));
        }

        if (!writeCursor.indices.empty())
        {
            parts.indexCount = static_cast<unsigned int>(writeCursor.indices.size());
            parts.indices._2 = memory.Alloc<uint16_t>(parts.indexCount);
            std::memcpy(parts.indices._2, writeCursor.indices.data(), parts.indexCount * sizeof(uint16_t));
        }
    }

    void WritePackedIndices(FlatDataWriteCursor& writeCursor, const std::vector<uint16_t>& indices, const bool useByteIndices)
    {
        const auto indexCount = indices.size();
        writeCursor.dataShort.emplace_back(static_cast<int16_t>(indexCount - 1)); // storedSize

        if (useByteIndices)
        {
            for (const auto index : indices)
            {
                assert(index <= std::numeric_limits<uint8_t>::max());
                writeCursor.dataByte.emplace_back(static_cast<uint8_t>(index));
            }
        }
        else if (indexCount >= 65)
        {
            // The linker moves 16-bit frame indices into the top-level indices pool only when
            // the in-memory stored size is at least 64, i.e. frameCount >= 65.
            std::ranges::copy(indices, std::back_inserter(writeCursor.indices));

            // The game inserts checkpoint values in dataShort
            // Those checkpoint values are copied from positions in the full index list: the first entry, then every 256th entry, and always the final entry.
            // The final entry is included even when it does not land exactly on a 256-entry boundary.
            const auto longTableSize = ((indexCount - 2) / 256u) + 1;
            for (auto i = 0u; i < longTableSize; i++)
                writeCursor.dataShort.emplace_back(indices[256 * i]);
            writeCursor.dataShort.emplace_back(indices[indices.size() - 1]);
        }
        else
        {
            std::ranges::copy(indices, std::back_inserter(writeCursor.dataShort));
        }
    }

    void ProcessQuatTrack(FlatDataWriteCursor& writeCursor, const QuatTrack& quatTrack, XAnimParts& parts, const bool useByteIndices)
    {
        switch (quatTrack.type)
        {
        case QuatType::NO_QUAT:
            parts.boneCount[PART_TYPE_NO_QUAT]++;
            break;

        case QuatType::HALF_QUAT:
            parts.boneCount[PART_TYPE_HALF_QUAT]++;
            WritePackedIndices(writeCursor, quatTrack.indices, useByteIndices);
            assert(quatTrack.values.size() == quatTrack.indices.size() * 2);
            std::ranges::copy(quatTrack.values, std::back_inserter(writeCursor.randomDataShort));
            break;

        case QuatType::FULL_QUAT:
            parts.boneCount[PART_TYPE_FULL_QUAT]++;
            WritePackedIndices(writeCursor, quatTrack.indices, useByteIndices);
            assert(quatTrack.values.size() == quatTrack.indices.size() * 4);
            std::ranges::copy(quatTrack.values, std::back_inserter(writeCursor.randomDataShort));
            break;

        case QuatType::HALF_QUAT_NO_SIZE:
            parts.boneCount[PART_TYPE_HALF_QUAT_NO_SIZE]++;
            assert(quatTrack.values.size() == 2);
            std::ranges::copy(quatTrack.values, std::back_inserter(writeCursor.dataShort));
            break;

        case QuatType::FULL_QUAT_NO_SIZE:
            parts.boneCount[PART_TYPE_FULL_QUAT_NO_SIZE]++;
            assert(quatTrack.values.size() == 4);
            std::ranges::copy(quatTrack.values, std::back_inserter(writeCursor.dataShort));
            break;
        }
    }

    void ProcessTransTrack(FlatDataWriteCursor& writeCursor, const TransTrack& transTrack, const size_t boneIndex, XAnimParts& parts, const bool useByteIndices)
    {
        assert(boneIndex <= std::numeric_limits<uint8_t>::max());
        writeCursor.dataByte.emplace_back(static_cast<uint8_t>(boneIndex));

        switch (transTrack.type)
        {
        case TransType::SMALL_TRANS:
            parts.boneCount[PART_TYPE_SMALL_TRANS]++;
            WritePackedIndices(writeCursor, transTrack.indices, useByteIndices);
            WriteFloat3(writeCursor, transTrack.mins);
            WriteFloat3(writeCursor, transTrack.size);
            assert(transTrack.byteFrames.size() == transTrack.indices.size() * 3);
            std::ranges::copy(transTrack.byteFrames, std::back_inserter(writeCursor.randomDataByte));
            break;

        case TransType::FULL_TRANS:
            parts.boneCount[PART_TYPE_TRANS]++;
            WritePackedIndices(writeCursor, transTrack.indices, useByteIndices);
            WriteFloat3(writeCursor, transTrack.mins);
            WriteFloat3(writeCursor, transTrack.size);
            assert(transTrack.shortFrames.size() == transTrack.indices.size() * 3);
            std::ranges::copy(transTrack.shortFrames, std::back_inserter(writeCursor.randomDataShort));
            break;

        case TransType::TRANS_NO_SIZE:
            parts.boneCount[PART_TYPE_TRANS_NO_SIZE]++;
            WriteFloat3(writeCursor, transTrack.constant);
            break;

        case TransType::NO_TRANS:
            parts.boneCount[PART_TYPE_NO_TRANS]++;
            break;
        }
    }

    class XAnimLoader final : public AssetCreator<AssetXAnim>
    {
    public:
        XAnimLoader(MemoryManager& memory, ISearchPath& searchPath, ZoneScriptStrings& scriptStrings)
            : m_memory(memory),
              m_search_path(searchPath),
              m_script_strings(scriptStrings)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(xanim::GetCompiledFileNameForAssetName(assetName));
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* parts = m_memory.Alloc<XAnimParts>();
            parts->name = m_memory.Dup(assetName.c_str());

            AssetRegistration<AssetXAnim> registration(assetName, parts);
            if (!LoadFromFile(*file.m_stream, *parts, registration))
            {
                con::error("Failed to load xanim \"{}\"", assetName);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        void ReadNoteTracks(std::istream& stream, XAnimParts& parts, AssetRegistration<AssetXAnim>& registration) const
        {
            const auto numDiskNoteTracks = stream::ReadValue<uint8_t>(stream);
            assert(numDiskNoteTracks + 1 <= std::numeric_limits<uint8_t>::max());

            uint8_t numNoteTracks;
            if (numDiskNoteTracks == std::numeric_limits<uint8_t>::max())
            {
                PrintError(parts, "Could not add \"end\" notify as maximum notify entries were reached");
                numNoteTracks = numDiskNoteTracks;
            }
            else
                numNoteTracks = numDiskNoteTracks + 1;

            parts.notifyCount = numNoteTracks;
            parts.notify = m_memory.Alloc<XAnimNotifyInfo>(numNoteTracks);

            for (auto notifyIndex = 0u; notifyIndex < numDiskNoteTracks; notifyIndex++)
            {
                auto& notify = parts.notify[notifyIndex];

                const auto notifyName = stream::ReadCString(stream);
                notify.name = m_script_strings.AddOrGetScriptString(notifyName);
                registration.AddScriptString(notify.name);

                const auto frame = stream::ReadValue<uint16_t>(stream);
                notify.time = parts.numframes > 0 ? static_cast<float>(frame) / static_cast<float>(parts.numframes) : 0;
                assert(notify.time >= 0.0f && notify.time <= 1.0f);
            }

            if (numNoteTracks > numDiskNoteTracks)
            {
                const auto endScriptString = m_script_strings.AddOrGetScriptString("end");
                registration.AddScriptString(endScriptString);
                parts.notify[numDiskNoteTracks].name = endScriptString;
                parts.notify[numDiskNoteTracks].time = 1.0f;
            }
        }

        void LoadDeltaQuats(std::istream& stream, XAnimDeltaPart& delta, const bool useByteIndices, const uint16_t numLoopFrames) const
        {
            const auto numQuatIndices = stream::ReadValue<uint16_t>(stream);
            if (numQuatIndices == 0)
                return;

            if (numQuatIndices == 1)
            {
                delta.quat = static_cast<XAnimDeltaPartQuat*>(m_memory.AllocRaw(offsetof(XAnimDeltaPartQuat, u) + sizeof(XAnimDeltaPartQuatData::frame0)));
                delta.quat->size = 0;
                ConsumeQuat2(stream, delta.quat->u.frame0);
                return;
            }

            const auto indicesArraySize =
                useByteIndices ? numQuatIndices * sizeof(XAnimDynamicIndicesQuat::_1) : numQuatIndices * sizeof(XAnimDynamicIndicesQuat::_2);

            delta.quat = static_cast<XAnimDeltaPartQuat*>(
                m_memory.AllocRaw(offsetof(XAnimDeltaPartQuat, u) + offsetof(XAnimDeltaPartQuatDataFrames, indices) + indicesArraySize));

            auto& quatIndices = delta.quat->u.frames.indices;
            LoadIndicesIfNeeded(stream, quatIndices, numQuatIndices, useByteIndices, numLoopFrames);

            delta.quat->size = static_cast<uint16_t>(numQuatIndices - 1);
            delta.quat->u.frames.frames = m_memory.Alloc<XQuat2>(numQuatIndices);

            for (auto quatIndexNum = 0u; quatIndexNum < numQuatIndices; ++quatIndexNum)
            {
                auto& curFrame = delta.quat->u.frames.frames[quatIndexNum];
                ConsumeQuat2(stream, curFrame);

                if (quatIndexNum > 0)
                {
                    const auto& prevFrame = delta.quat->u.frames.frames[quatIndexNum - 1];
                    if (prevFrame.value[0] * curFrame.value[0] + prevFrame.value[1] * curFrame.value[1] < 0)
                        FlipQuat2(curFrame);
                }
            }
        }

        void LoadDeltaTrans(std::istream& stream, XAnimDeltaPart& delta, const bool useByteIndices, const uint16_t numLoopFrames) const
        {
            const auto numTransIndices = stream::ReadValue<uint16_t>(stream);
            if (numTransIndices == 0)
                return;

            if (numTransIndices == 1)
            {
                delta.trans = static_cast<XAnimPartTrans*>(m_memory.AllocRaw(offsetof(XAnimPartTrans, u) + sizeof(XAnimPartTransData::frame0)));
                delta.trans->size = 0;
                delta.trans->u.frame0.x = stream::ReadValue<float>(stream);
                delta.trans->u.frame0.y = stream::ReadValue<float>(stream);
                delta.trans->u.frame0.z = stream::ReadValue<float>(stream);
                return;
            }
            const auto indicesArraySize =
                useByteIndices ? numTransIndices * sizeof(XAnimDynamicIndicesTrans::_1) : numTransIndices * sizeof(XAnimDynamicIndicesTrans::_2);

            delta.trans =
                static_cast<XAnimPartTrans*>(m_memory.AllocRaw(offsetof(XAnimPartTrans, u) + offsetof(XAnimPartTransFrames, indices) + indicesArraySize));

            auto& frames = delta.trans->u.frames;
            LoadIndicesIfNeeded(stream, frames.indices, numTransIndices, useByteIndices, numLoopFrames);

            const auto smallTrans = stream::ReadValue<bool>(stream);
            delta.trans->smallTrans = smallTrans ? 1 : 0;

            frames.mins.x = stream::ReadValue<float>(stream);
            frames.mins.y = stream::ReadValue<float>(stream);
            frames.mins.z = stream::ReadValue<float>(stream);

            frames.size.x = DecodeRawTransSize(stream::ReadValue<float>(stream), smallTrans);
            frames.size.y = DecodeRawTransSize(stream::ReadValue<float>(stream), smallTrans);
            frames.size.z = DecodeRawTransSize(stream::ReadValue<float>(stream), smallTrans);

            delta.trans->size = static_cast<uint16_t>(numTransIndices - 1);
            if (smallTrans)
            {
                frames.frames._1 = m_memory.Alloc<ByteVec>(numTransIndices);
                stream::Read(stream, frames.frames._1, numTransIndices * sizeof(ByteVec));
            }
            else
            {
                frames.frames._2 = m_memory.Alloc<UShortVec>(numTransIndices);
                stream::Read(stream, frames.frames._2, numTransIndices * sizeof(UShortVec));
            }
        }

        void LoadDeltaTrack(std::istream& stream, XAnimParts& parts, const bool useByteIndices, const uint16_t numLoopFrames) const
        {
            auto* delta = m_memory.Alloc<XAnimDeltaPart>();
            parts.deltaPart = delta;

            LoadDeltaQuats(stream, *delta, useByteIndices, numLoopFrames);
            LoadDeltaTrans(stream, *delta, useByteIndices, numLoopFrames);
        }

        bool LoadFromFile(std::istream& stream, XAnimParts& parts, AssetRegistration<AssetXAnim>& registration) const
        {
            const auto fileVersion = stream::ReadValue<uint16_t>(stream);
            if (fileVersion != RAW_VERSION)
            {
                PrintError(parts, std::format("Unsupported version number {} (expected {})", fileVersion, RAW_VERSION));
                return false;
            }

            const auto numFrames = stream::ReadValue<uint16_t>(stream);
            const auto boneCount = stream::ReadValue<uint16_t>(stream);
            const auto flags = stream::ReadValue<uint8_t>(stream);
            const auto assetType = stream::ReadValue<uint8_t>(stream);
            const auto framerate = stream::ReadValue<uint16_t>(stream);
            if (stream.fail())
            {
                PrintError(parts, "Truncated file");
                return false;
            }

            const bool isLooped = flags & FLAG_LOOPED;
            const bool hasDelta = flags & FLAG_DELTA;
            const uint16_t numLoopFrames = isLooped ? numFrames + 1u : numFrames;

            parts.numframes = numLoopFrames - 1;
            parts.bLoop = isLooped;
            parts.bDelta = hasDelta;
            parts.assetType = assetType;
            parts.framerate = static_cast<float>(framerate);
            parts.frequency = parts.numframes > 0 ? parts.framerate / static_cast<float>(parts.numframes) : 0;

            const auto useByteIndices = UseByteIndices(parts);

            if (hasDelta)
                LoadDeltaTrack(stream, parts, useByteIndices, numLoopFrames);

            std::vector<BoneTrack> boneTracks;
            if (boneCount > 0)
            {
                const auto bitmaskSize = utils::Align<size_t>(boneCount, 8u) / 8u;
                std::vector<uint8_t> flipQuatBits(bitmaskSize, 0);
                std::vector<uint8_t> halfQuatBits(bitmaskSize, 0);
                stream::Read(stream, flipQuatBits.data(), bitmaskSize);
                stream::Read(stream, halfQuatBits.data(), bitmaskSize);

                boneTracks.resize(boneCount);
                for (size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex)
                    boneTracks[boneIndex].name = stream::ReadCString(stream);

                for (size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex)
                {
                    auto& boneTrack = boneTracks[boneIndex];

                    const bool flipQuat = flipQuatBits[boneIndex / 8u] & static_cast<uint8_t>(1u << (boneIndex % 8u));
                    const bool halfQuat = halfQuatBits[boneIndex / 8u] & static_cast<uint8_t>(1u << (boneIndex % 8u));

                    ReadQuatTrack(stream, boneTrack.quat, numLoopFrames, useByteIndices, flipQuat, halfQuat);
                    ReadTransTrack(stream, boneTrack.trans, numLoopFrames, useByteIndices);
                }
            }

            ReadNoteTracks(stream, parts, registration);

            FlatDataWriteCursor writeCursor;

            std::vector<size_t> boneOrder(boneCount);
            std::ranges::iota(boneOrder, 0);

            std::ranges::sort(boneOrder,
                              [&boneTracks](const size_t i0, const size_t i1)
                              {
                                  const auto type0 = std::to_underlying(boneTracks[i0].quat.type);
                                  const auto type1 = std::to_underlying(boneTracks[i1].quat.type);
                                  if (type0 != type1)
                                      return type0 < type1;

                                  return i0 < i1;
                              });

            // The parts bone indices are based on the quats order
            std::vector<size_t> boneTrackIndexToPartsBoneIndex(boneCount);
            parts.names = m_memory.Alloc<ScriptString>(boneCount);
            for (auto partsBoneIndex = 0u; partsBoneIndex < boneCount; ++partsBoneIndex)
            {
                const auto boneTrackIndex = boneOrder[partsBoneIndex];
                boneTrackIndexToPartsBoneIndex[boneTrackIndex] = partsBoneIndex;
                ProcessQuatTrack(writeCursor, boneTracks[boneTrackIndex].quat, parts, useByteIndices);

                // Names are based on quats order so apply them here as well
                const auto scrString = m_script_strings.AddOrGetScriptString(boneTracks[boneTrackIndex].name);
                parts.names[partsBoneIndex] = scrString;
                registration.AddScriptString(scrString);
            }

            // Trans are ordered differently
            std::ranges::sort(boneOrder,
                              [&boneTracks, &boneTrackIndexToPartsBoneIndex](const size_t i0, const size_t i1)
                              {
                                  const auto type0 = std::to_underlying(boneTracks[i0].trans.type);
                                  const auto type1 = std::to_underlying(boneTracks[i1].trans.type);
                                  if (type0 != type1)
                                      return type0 < type1;

                                  return boneTrackIndexToPartsBoneIndex[i0] < boneTrackIndexToPartsBoneIndex[i1];
                              });
            for (auto partsBoneIndex = 0u; partsBoneIndex < boneCount; ++partsBoneIndex)
            {
                const auto boneTrackIndex = boneOrder[partsBoneIndex];
                ProcessTransTrack(writeCursor, boneTracks[boneTrackIndex].trans, boneTrackIndexToPartsBoneIndex[boneTrackIndex], parts, useByteIndices);
            }

            ApplyWriteCursorToParts(parts, writeCursor, m_memory);
            parts.boneCount[PART_TYPE_ALL] = static_cast<uint8_t>(boneCount);

            assert(stream.peek() == std::char_traits<char>::eof());
            return true;
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        ZoneScriptStrings& m_script_strings;
    };
} // namespace

namespace xanim
{
    std::unique_ptr<AssetCreator<AssetXAnim>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<XAnimLoader>(memory, searchPath, zone.m_script_strings);
    }
} // namespace xanim
