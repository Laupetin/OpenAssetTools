#include "XAnimDumperIW3.h"

#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace IW3;

namespace
{
    constexpr auto RAW_VERSION = static_cast<uint16_t>(17);
    // The linker decodes raw trans size[] with these exact float literals.
    // They correspond to 1.0f / 255.0f and 1.0f / 65535.0f, but we keep the
    // decompiled values to preserve binary-stable round trips.
    constexpr auto SMALL_TRANS_SIZE_SCALE = 0.003921568859368563f;
    constexpr auto FULL_TRANS_SIZE_SCALE = 0.00001525902189314365f;

    enum class QuatType : uint8_t
    {
        IDENTITY = 0,
        SIMPLE_KEYFRAMED = 1,
        FULL_KEYFRAMED = 2,
        SIMPLE_CONSTANT = 3,
        FULL_CONSTANT = 4,
    };

    enum class TransType : uint8_t
    {
        SIMPLE_KEYFRAMED = 5,
        FULL_KEYFRAMED = 6,
        CONSTANT = 7,
        IDENTITY = 8,
    };

    struct QuatTrack
    {
        QuatType type = QuatType::IDENTITY;
        std::vector<uint16_t> indices;
        std::vector<int16_t> values;
    };

    struct TransTrack
    {
        TransType type = TransType::IDENTITY;
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

    struct FlatDataCursor
    {
        const uint8_t* dataByte;
        const int16_t* dataShort;
        const int* dataInt;
        const uint8_t* randomDataByte;
        const int16_t* randomDataShort;
        const uint16_t* indices;
    };

    struct DeltaQuatTrack
    {
        std::vector<uint16_t> indices;
        std::vector<int16_t> values;
    };

    struct DeltaTransTrack
    {
        bool exists = false;
        bool keyframed = false;
        bool smallTrans = false;
        std::vector<uint16_t> indices;
        std::array<float, 3> mins{};
        std::array<float, 3> size{};
        std::vector<uint8_t> byteFrames;
        std::vector<uint16_t> shortFrames;
        std::array<float, 3> constant{};
    };

    struct DeltaTrack
    {
        bool hasQuat = false;
        bool quatKeyframed = false;
        DeltaQuatTrack quat;
        DeltaTransTrack trans;
    };

    struct EncodedQuatTrack
    {
        bool flipQuat = false;
        std::vector<int16_t> storedValues;
    };

    [[nodiscard]] std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return "xanim/" + assetName;
    }

    [[nodiscard]] const std::string& ResolveScriptString(const XAssetInfo<XAnimParts>& asset, const ScriptString value)
    {
        assert(asset.m_zone != nullptr && value < asset.m_zone->m_script_strings.Count());
        return asset.m_zone->m_script_strings[value];
    }

    [[nodiscard]] uint16_t GetNumLoopFrames(const XAnimParts& parts)
    {
        assert(parts.numframes < std::numeric_limits<uint16_t>::max());
        // Raw non-looped xanims store numframes + 1 in keyed track counts/header fields.
        return static_cast<uint16_t>(parts.numframes + 1u);
    }

    [[nodiscard]] bool UseByteIndices(const XAnimParts& parts)
    {
        return parts.numframes < 256;
    }

    template<typename T> void WriteValue(std::ostream& stream, const T& value)
    {
        stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    void WriteCString(std::ostream& stream, const std::string& value)
    {
        stream.write(value.c_str(), static_cast<std::streamsize>(value.size() + 1));
    }

    [[nodiscard]] float IntBitsToFloat(const int value)
    {
        float result;
        static_assert(sizeof(result) == sizeof(value));
        std::memcpy(&result, &value, sizeof(result));
        return result;
    }

    [[nodiscard]] std::array<float, 3> ReadFloat3(const int*& dataInt)
    {
        std::array<float, 3> result{};
        for (auto i = 0uz; i < result.size(); i++)
            result[i] = IntBitsToFloat(*dataInt++);
        return result;
    }

    template<typename T> [[nodiscard]] const T* AdvancePtr(const T* ptr, const size_t count)
    {
        if (count == 0uz)
            return ptr;

        assert(ptr != nullptr);
        return ptr + count;
    }

    [[nodiscard]] std::vector<uint16_t> ReadPackedIndices(FlatDataCursor& cursor, const uint16_t storedSize, const bool useByteIndices)
    {
        const auto count = static_cast<size_t>(storedSize) + 1uz;
        std::vector<uint16_t> result(count);

        if (useByteIndices)
        {
            for (auto i = 0uz; i < count; i++)
                result[i] = cursor.dataByte[i];

            cursor.dataByte += count;
            return result;
        }

        // The linker moves 16-bit frame indices into the top-level indices pool only when
        // the in-memory stored size is at least 64, i.e. frameCount >= 65.
        if (storedSize >= 64u)
        {
            for (auto i = 0uz; i < count; i++)
                result[i] = cursor.indices[i];

            cursor.indices += count;
            cursor.dataShort += ((count - 2uz) >> 8u) + 2uz;
            return result;
        }

        for (auto i = 0uz; i < count; i++)
            result[i] = static_cast<uint16_t>(cursor.dataShort[i]);

        cursor.dataShort += count;
        return result;
    }

    [[nodiscard]] bool IsSequentialCoverage(const std::vector<uint16_t>& indices, const uint16_t numLoopFrames)
    {
        if (indices.size() != numLoopFrames)
            return false;

        for (auto i = 0uz; i < indices.size(); i++)
        {
            if (indices[i] != i)
                return false;
        }

        return true;
    }

    [[nodiscard]] bool QuatTypeUsesSimpleMask(const QuatType type)
    {
        return type == QuatType::IDENTITY || type == QuatType::SIMPLE_KEYFRAMED || type == QuatType::SIMPLE_CONSTANT;
    }

    [[nodiscard]] float EncodeRawTransSize(const float value, const bool smallTrans)
    {
        const auto scale = smallTrans ? SMALL_TRANS_SIZE_SCALE : FULL_TRANS_SIZE_SCALE;
        return value / scale;
    }

    [[nodiscard]] int64_t ComputeQuatDot(const int16_t* lhs, const int16_t* rhs, const size_t componentCount)
    {
        int64_t result = 0;
        for (auto i = 0uz; i < componentCount; i++)
            result += static_cast<int64_t>(lhs[i]) * static_cast<int64_t>(rhs[i]);

        return result;
    }

    [[nodiscard]] EncodedQuatTrack EncodeQuatFrames(const int16_t* values, const size_t frameCount, const size_t componentCount, const bool allowFlipQuat)
    {
        assert(componentCount == 2uz || componentCount == 4uz);

        EncodedQuatTrack result;
        if (frameCount == 0uz)
            return result;

        const auto storedComponentCount = componentCount - 1uz;
        result.storedValues.reserve(frameCount * storedComponentCount);

        // Raw IW3 xanims store only N-1 quat components. The loader reconstructs the
        // final component with a positive sqrt, applies the per-bone flip bit, and then
        // continuity-corrects subsequent frames by optionally negating whole quats.
        result.flipQuat = allowFlipQuat && values[storedComponentCount] < 0;
        const auto targetNegativeOmitted = result.flipQuat;

        for (auto frameIndex = 0uz; frameIndex < frameCount; frameIndex++)
        {
            const auto* frame = values + frameIndex * componentCount;
            const auto omittedNegative = frame[storedComponentCount] < 0;

            auto continuityNegated = false;
            if (frameIndex > 0uz && omittedNegative != targetNegativeOmitted)
            {
                const auto* prevFrame = values + (frameIndex - 1uz) * componentCount;
                continuityNegated = ComputeQuatDot(prevFrame, frame, componentCount) > 0;
            }

            const auto rawNegated = result.flipQuat != continuityNegated;
            const auto sign = rawNegated ? -1 : 1;

            for (auto componentIndex = 0uz; componentIndex < storedComponentCount; componentIndex++)
            {
                const auto value = static_cast<int>(frame[componentIndex]) * sign;
                assert(value >= std::numeric_limits<int16_t>::min() && value <= std::numeric_limits<int16_t>::max());
                result.storedValues.push_back(static_cast<int16_t>(value));
            }
        }

        return result;
    }

    [[nodiscard]] EncodedQuatTrack EncodeQuatTrack(const QuatTrack& quat)
    {
        switch (quat.type)
        {
        case QuatType::IDENTITY:
            return {};

        case QuatType::SIMPLE_CONSTANT:
            assert(quat.values.size() == 2uz);
            return EncodeQuatFrames(quat.values.data(), 1uz, 2uz, true);

        case QuatType::FULL_CONSTANT:
            assert(quat.values.size() == 4uz);
            return EncodeQuatFrames(quat.values.data(), 1uz, 4uz, true);

        case QuatType::SIMPLE_KEYFRAMED:
        {
            const auto frameCount = quat.indices.size();
            assert(quat.values.size() == frameCount * 2uz);
            return EncodeQuatFrames(quat.values.data(), frameCount, 2uz, true);
        }

        case QuatType::FULL_KEYFRAMED:
        {
            const auto frameCount = quat.indices.size();
            assert(quat.values.size() == frameCount * 4uz);
            return EncodeQuatFrames(quat.values.data(), frameCount, 4uz, true);
        }
        }

        assert(false);
        return {};
    }

    [[nodiscard]] EncodedQuatTrack EncodeDeltaQuatTrack(const DeltaTrack& delta)
    {
        if (!delta.hasQuat)
            return {};

        // Delta quats are serialized without the per-bone flipQuat mask used by normal bone quats.
        if (!delta.quatKeyframed)
        {
            assert(delta.quat.values.size() == 2uz);
            return EncodeQuatFrames(delta.quat.values.data(), 1uz, 2uz, false);
        }

        const auto frameCount = delta.quat.indices.size();
        assert(delta.quat.values.size() == frameCount * 2uz);
        return EncodeQuatFrames(delta.quat.values.data(), frameCount, 2uz, false);
    }

    [[nodiscard]] std::vector<BoneTrack> ReconstructBoneTracks(const XAssetInfo<XAnimParts>& asset)
    {
        const auto& parts = *asset.Asset();
        const auto nameCount = static_cast<size_t>(parts.boneCount[9]);
        const auto useByteIndices = UseByteIndices(parts);

        std::vector<BoneTrack> bones(nameCount);
        for (auto i = 0uz; i < nameCount; i++)
            bones[i].name = ResolveScriptString(asset, parts.names[i]);

        auto cursor = FlatDataCursor{
            reinterpret_cast<const uint8_t*>(parts.dataByte),
            parts.dataShort,
            parts.dataInt,
            reinterpret_cast<const uint8_t*>(parts.randomDataByte),
            parts.randomDataShort,
            parts.indices._2,
        };
        const auto failReconstruction = [&asset](const char* field)
        {
            throw std::runtime_error("IW3 xanim raw reconstruction cursor mismatch for asset \"" + asset.m_name + "\" in " + field);
        };

        size_t boneIndex = 0;

        for (auto i = 0u; i < parts.boneCount[0]; i++, boneIndex++)
            bones[boneIndex].quat.type = QuatType::IDENTITY;

        for (auto i = 0u; i < parts.boneCount[1]; i++, boneIndex++)
        {
            auto& quat = bones[boneIndex].quat;
            quat.type = QuatType::SIMPLE_KEYFRAMED;
            const auto storedSize = static_cast<uint16_t>(*cursor.dataShort++);
            const auto frameCount = static_cast<size_t>(storedSize) + 1uz;
            quat.indices = ReadPackedIndices(cursor, storedSize, useByteIndices);
            quat.values.assign(cursor.randomDataShort, cursor.randomDataShort + frameCount * 2uz);
            cursor.randomDataShort += frameCount * 2uz;
        }

        for (auto i = 0u; i < parts.boneCount[2]; i++, boneIndex++)
        {
            auto& quat = bones[boneIndex].quat;
            quat.type = QuatType::FULL_KEYFRAMED;
            const auto storedSize = static_cast<uint16_t>(*cursor.dataShort++);
            const auto frameCount = static_cast<size_t>(storedSize) + 1uz;
            quat.indices = ReadPackedIndices(cursor, storedSize, useByteIndices);
            quat.values.assign(cursor.randomDataShort, cursor.randomDataShort + frameCount * 4uz);
            cursor.randomDataShort += frameCount * 4uz;
        }

        for (auto i = 0u; i < parts.boneCount[3]; i++, boneIndex++)
        {
            auto& quat = bones[boneIndex].quat;
            quat.type = QuatType::SIMPLE_CONSTANT;
            quat.values.assign(cursor.dataShort, cursor.dataShort + 2);
            cursor.dataShort += 2;
        }

        for (auto i = 0u; i < parts.boneCount[4]; i++, boneIndex++)
        {
            auto& quat = bones[boneIndex].quat;
            quat.type = QuatType::FULL_CONSTANT;
            quat.values.assign(cursor.dataShort, cursor.dataShort + 4);
            cursor.dataShort += 4;
        }

        std::vector<bool> transAssigned(nameCount, false);

        for (auto i = 0u; i < parts.boneCount[5]; i++)
        {
            const auto bone = static_cast<size_t>(*cursor.dataByte++);
            assert(bone < nameCount && !transAssigned[bone]);

            auto& trans = bones[bone].trans;
            transAssigned[bone] = true;
            trans.type = TransType::SIMPLE_KEYFRAMED;

            const auto storedSize = static_cast<uint16_t>(*cursor.dataShort++);
            const auto frameCount = static_cast<size_t>(storedSize) + 1uz;
            trans.mins = ReadFloat3(cursor.dataInt);
            trans.size = ReadFloat3(cursor.dataInt);
            trans.indices = ReadPackedIndices(cursor, storedSize, useByteIndices);
            trans.byteFrames.assign(cursor.randomDataByte, cursor.randomDataByte + frameCount * 3uz);
            cursor.randomDataByte += frameCount * 3uz;
        }

        for (auto i = 0u; i < parts.boneCount[6]; i++)
        {
            const auto bone = static_cast<size_t>(*cursor.dataByte++);
            assert(bone < nameCount && !transAssigned[bone]);

            auto& trans = bones[bone].trans;
            transAssigned[bone] = true;
            trans.type = TransType::FULL_KEYFRAMED;

            const auto storedSize = static_cast<uint16_t>(*cursor.dataShort++);
            const auto frameCount = static_cast<size_t>(storedSize) + 1uz;
            trans.mins = ReadFloat3(cursor.dataInt);
            trans.size = ReadFloat3(cursor.dataInt);
            trans.indices = ReadPackedIndices(cursor, storedSize, useByteIndices);
            trans.shortFrames.reserve(frameCount * 3uz);
            for (auto frame = 0uz; frame < frameCount * 3uz; frame++)
                trans.shortFrames.push_back(static_cast<uint16_t>(*cursor.randomDataShort++));
        }

        for (auto i = 0u; i < parts.boneCount[7]; i++)
        {
            const auto bone = static_cast<size_t>(*cursor.dataByte++);
            assert(bone < nameCount && !transAssigned[bone]);

            auto& trans = bones[bone].trans;
            transAssigned[bone] = true;
            trans.type = TransType::CONSTANT;
            trans.constant = ReadFloat3(cursor.dataInt);
        }

        for (auto i = 0u; i < parts.boneCount[8]; i++)
        {
            const auto bone = static_cast<size_t>(*cursor.dataByte++);
            assert(bone < nameCount && !transAssigned[bone]);

            bones[bone].trans.type = TransType::IDENTITY;
            transAssigned[bone] = true;
        }

        for (auto i = 0uz; i < nameCount; i++)
            assert(transAssigned[i]);

        const auto dataByteEnd = AdvancePtr(reinterpret_cast<const uint8_t*>(parts.dataByte), parts.dataByteCount);
        const auto dataShortEnd = AdvancePtr(parts.dataShort, parts.dataShortCount);
        const auto dataIntEnd = AdvancePtr(parts.dataInt, parts.dataIntCount);
        const auto randomDataByteEnd = AdvancePtr(reinterpret_cast<const uint8_t*>(parts.randomDataByte), parts.randomDataByteCount);
        const auto randomDataShortEnd = AdvancePtr(parts.randomDataShort, parts.randomDataShortCount);

        if (cursor.dataByte != dataByteEnd)
            failReconstruction("dataByte");
        if (cursor.dataShort != dataShortEnd)
            failReconstruction("dataShort");
        if (cursor.dataInt != dataIntEnd)
            failReconstruction("dataInt");
        if (cursor.randomDataByte != randomDataByteEnd)
            failReconstruction("randomDataByte");
        if (cursor.randomDataShort != randomDataShortEnd)
            failReconstruction("randomDataShort");

        if (!useByteIndices)
        {
            const auto indicesEnd = AdvancePtr(parts.indices._2, parts.indexCount);
            if (cursor.indices != indicesEnd)
                failReconstruction("indices");
        }
        else
        {
            assert(parts.indexCount == 0);
        }

        return bones;
    }

    [[nodiscard]] DeltaTrack ReconstructDeltaTrack(const XAnimParts& parts)
    {
        DeltaTrack result;
        if (!parts.deltaPart)
            return result;

        const auto numLoopFrames = GetNumLoopFrames(parts);
        const auto useByteIndices = UseByteIndices(parts);

        if (const auto* quat = parts.deltaPart->quat; quat)
        {
            result.hasQuat = true;

            if (quat->size > 0)
            {
                result.quatKeyframed = true;
                const auto frameCount = static_cast<size_t>(quat->size) + 1uz;
                result.quat.values.reserve(frameCount * 2uz);
                result.quat.indices.reserve(frameCount);

                for (auto i = 0uz; i < frameCount; i++)
                {
                    result.quat.values.push_back(quat->u.frames.frames[i].value[0]);
                    result.quat.values.push_back(quat->u.frames.frames[i].value[1]);
                }

                if (useByteIndices)
                {
                    for (auto i = 0uz; i < frameCount; i++)
                        result.quat.indices.push_back(static_cast<uint8_t>(quat->u.frames.indices._1[i]));
                }
                else
                {
                    for (auto i = 0uz; i < frameCount; i++)
                        result.quat.indices.push_back(quat->u.frames.indices._2[i]);
                }

                assert(result.quat.indices.size() <= numLoopFrames);
            }
            else
            {
                result.quat.values.push_back(quat->u.frame0.value[0]);
                result.quat.values.push_back(quat->u.frame0.value[1]);
            }
        }

        if (const auto* trans = parts.deltaPart->trans; trans)
        {
            result.trans.exists = true;

            if (trans->size > 0)
            {
                result.trans.keyframed = true;
                result.trans.smallTrans = trans->smallTrans;
                result.trans.mins = {trans->u.frames.mins[0], trans->u.frames.mins[1], trans->u.frames.mins[2]};
                result.trans.size = {trans->u.frames.size[0], trans->u.frames.size[1], trans->u.frames.size[2]};

                const auto frameCount = static_cast<size_t>(trans->size) + 1uz;
                result.trans.indices.reserve(frameCount);
                if (useByteIndices)
                {
                    for (auto i = 0uz; i < frameCount; i++)
                        result.trans.indices.push_back(static_cast<uint8_t>(trans->u.frames.indices._1[i]));
                }
                else
                {
                    for (auto i = 0uz; i < frameCount; i++)
                        result.trans.indices.push_back(trans->u.frames.indices._2[i]);
                }

                if (trans->smallTrans)
                {
                    result.trans.byteFrames.reserve(frameCount * 3uz);
                    for (auto i = 0uz; i < frameCount; i++)
                    {
                        result.trans.byteFrames.push_back(trans->u.frames.frames._1[i][0]);
                        result.trans.byteFrames.push_back(trans->u.frames.frames._1[i][1]);
                        result.trans.byteFrames.push_back(trans->u.frames.frames._1[i][2]);
                    }
                }
                else
                {
                    result.trans.shortFrames.reserve(frameCount * 3uz);
                    for (auto i = 0uz; i < frameCount; i++)
                    {
                        result.trans.shortFrames.push_back(trans->u.frames.frames._2[i][0]);
                        result.trans.shortFrames.push_back(trans->u.frames.frames._2[i][1]);
                        result.trans.shortFrames.push_back(trans->u.frames.frames._2[i][2]);
                    }
                }
            }
            else
            {
                result.trans.constant = {trans->u.frame0.v[0], trans->u.frame0.v[1], trans->u.frame0.v[2]};
            }
        }

        return result;
    }

    void WriteIndicesIfNeeded(std::ostream& stream, const std::vector<uint16_t>& indices, const uint16_t numLoopFrames, const bool useByteIndices)
    {
        if (indices.empty())
            return;

        // The raw format omits indices when a track covers every loop frame in order.
        if (indices.size() >= numLoopFrames)
        {
            assert(IsSequentialCoverage(indices, numLoopFrames));
            return;
        }

        if (useByteIndices)
        {
            for (const auto index : indices)
            {
                assert(index <= std::numeric_limits<uint8_t>::max());
                const auto asByte = static_cast<uint8_t>(index);
                WriteValue(stream, asByte);
            }
        }
        else
        {
            for (const auto index : indices)
                WriteValue(stream, index);
        }
    }

    void WriteQuatTrack(
        std::ostream& stream, const QuatTrack& quat, const EncodedQuatTrack& encodedQuat, const uint16_t numLoopFrames, const bool useByteIndices)
    {
        switch (quat.type)
        {
        case QuatType::IDENTITY:
        {
            WriteValue(stream, static_cast<uint16_t>(0));
            break;
        }

        case QuatType::SIMPLE_CONSTANT:
        {
            assert(encodedQuat.storedValues.size() == 1uz);
            WriteValue(stream, static_cast<uint16_t>(1));
            WriteValue(stream, encodedQuat.storedValues[0]);
            break;
        }

        case QuatType::FULL_CONSTANT:
        {
            assert(encodedQuat.storedValues.size() == 3uz);
            WriteValue(stream, static_cast<uint16_t>(1));
            for (const auto value : encodedQuat.storedValues)
                WriteValue(stream, value);
            break;
        }

        case QuatType::SIMPLE_KEYFRAMED:
        {
            const auto frameCount = quat.indices.size();
            assert(frameCount > 0uz);
            assert(quat.values.size() == frameCount * 2uz);
            assert(encodedQuat.storedValues.size() == frameCount);

            WriteValue(stream, static_cast<uint16_t>(frameCount));
            WriteIndicesIfNeeded(stream, quat.indices, numLoopFrames, useByteIndices);
            for (const auto value : encodedQuat.storedValues)
                WriteValue(stream, value);
            break;
        }

        case QuatType::FULL_KEYFRAMED:
        {
            const auto frameCount = quat.indices.size();
            assert(frameCount > 0uz);
            assert(quat.values.size() == frameCount * 4uz);
            assert(encodedQuat.storedValues.size() == frameCount * 3uz);

            WriteValue(stream, static_cast<uint16_t>(frameCount));
            WriteIndicesIfNeeded(stream, quat.indices, numLoopFrames, useByteIndices);
            for (const auto value : encodedQuat.storedValues)
                WriteValue(stream, value);
            break;
        }
        }
    }

    void WriteTransTrack(std::ostream& stream, const TransTrack& trans, const uint16_t numLoopFrames, const bool useByteIndices)
    {
        switch (trans.type)
        {
        case TransType::IDENTITY:
        {
            WriteValue(stream, static_cast<uint16_t>(0));
            break;
        }

        case TransType::CONSTANT:
        {
            WriteValue(stream, static_cast<uint16_t>(1));
            for (const auto value : trans.constant)
                WriteValue(stream, value);
            break;
        }

        case TransType::SIMPLE_KEYFRAMED:
        {
            const auto frameCount = trans.indices.size();
            assert(frameCount > 0uz);
            assert(trans.byteFrames.size() == frameCount * 3uz);

            WriteValue(stream, static_cast<uint16_t>(frameCount));
            WriteIndicesIfNeeded(stream, trans.indices, numLoopFrames, useByteIndices);

            const auto smallTrans = static_cast<uint8_t>(1);
            WriteValue(stream, smallTrans);
            for (const auto value : trans.mins)
                WriteValue(stream, value);
            for (const auto value : trans.size)
                WriteValue(stream, EncodeRawTransSize(value, true));

            stream.write(reinterpret_cast<const char*>(trans.byteFrames.data()), static_cast<std::streamsize>(trans.byteFrames.size()));
            break;
        }

        case TransType::FULL_KEYFRAMED:
        {
            const auto frameCount = trans.indices.size();
            assert(frameCount > 0uz);
            assert(trans.shortFrames.size() == frameCount * 3uz);

            WriteValue(stream, static_cast<uint16_t>(frameCount));
            WriteIndicesIfNeeded(stream, trans.indices, numLoopFrames, useByteIndices);

            const auto smallTrans = static_cast<uint8_t>(0);
            WriteValue(stream, smallTrans);
            for (const auto value : trans.mins)
                WriteValue(stream, value);
            for (const auto value : trans.size)
                WriteValue(stream, EncodeRawTransSize(value, false));

            for (const auto value : trans.shortFrames)
                WriteValue(stream, value);
            break;
        }
        }
    }

    void WriteDeltaTrack(std::ostream& stream, const DeltaTrack& delta, const uint16_t numLoopFrames, const bool useByteIndices)
    {
        const auto encodedDeltaQuat = EncodeDeltaQuatTrack(delta);

        if (!delta.hasQuat)
        {
            WriteValue(stream, static_cast<uint16_t>(0));
        }
        else if (!delta.quatKeyframed)
        {
            assert(encodedDeltaQuat.storedValues.size() == 1uz);
            WriteValue(stream, static_cast<uint16_t>(1));
            WriteValue(stream, encodedDeltaQuat.storedValues[0]);
        }
        else
        {
            const auto frameCount = delta.quat.indices.size();
            assert(frameCount > 0uz);
            assert(delta.quat.values.size() == frameCount * 2uz);
            assert(encodedDeltaQuat.storedValues.size() == frameCount);

            WriteValue(stream, static_cast<uint16_t>(frameCount));
            WriteIndicesIfNeeded(stream, delta.quat.indices, numLoopFrames, useByteIndices);
            for (const auto value : encodedDeltaQuat.storedValues)
                WriteValue(stream, value);
        }

        if (!delta.trans.exists)
        {
            WriteValue(stream, static_cast<uint16_t>(0));
            return;
        }

        if (!delta.trans.keyframed)
        {
            WriteValue(stream, static_cast<uint16_t>(1));
            for (const auto value : delta.trans.constant)
                WriteValue(stream, value);
            return;
        }

        const auto frameCount = delta.trans.indices.size();
        assert(frameCount > 0uz);

        WriteValue(stream, static_cast<uint16_t>(frameCount));
        WriteIndicesIfNeeded(stream, delta.trans.indices, numLoopFrames, useByteIndices);

        const auto smallTrans = static_cast<uint8_t>(delta.trans.smallTrans ? 1 : 0);
        WriteValue(stream, smallTrans);
        for (const auto value : delta.trans.mins)
            WriteValue(stream, value);

        if (delta.trans.smallTrans)
        {
            assert(delta.trans.byteFrames.size() == frameCount * 3uz);
            for (const auto value : delta.trans.size)
                WriteValue(stream, EncodeRawTransSize(value, true));

            stream.write(reinterpret_cast<const char*>(delta.trans.byteFrames.data()), static_cast<std::streamsize>(delta.trans.byteFrames.size()));
        }
        else
        {
            assert(delta.trans.shortFrames.size() == frameCount * 3uz);
            for (const auto value : delta.trans.size)
                WriteValue(stream, EncodeRawTransSize(value, false));

            for (const auto value : delta.trans.shortFrames)
                WriteValue(stream, value);
        }
    }

    void WriteNotifyTail(std::ostream& stream, const XAssetInfo<XAnimParts>& asset)
    {
        const auto& parts = *asset.Asset();
        const auto notifyCount = static_cast<size_t>(static_cast<unsigned char>(parts.notifyCount));

        size_t rawNotifyCount = notifyCount;
        if (notifyCount > 0uz)
        {
            const auto& lastName = ResolveScriptString(asset, parts.notify[notifyCount - 1].name);
            const auto lastTime = parts.notify[notifyCount - 1].time;

            // The linker appends a synthetic "end" notify at 1.0f to the loaded asset state.
            if (lastName == "end" && std::abs(lastTime - 1.0f) < 0.0001f)
                rawNotifyCount--;
        }

        assert(rawNotifyCount < 255uz);
        const auto rawNotifyCountByte = static_cast<uint8_t>(rawNotifyCount);
        WriteValue(stream, rawNotifyCountByte);

        for (auto i = 0uz; i < rawNotifyCount; i++)
        {
            WriteCString(stream, ResolveScriptString(asset, parts.notify[i].name));

            uint16_t frame = 0;
            if (parts.numframes > 0)
            {
                const auto scaled = static_cast<long>(std::lround(parts.notify[i].time * static_cast<float>(parts.numframes)));
                assert(scaled >= 0 && scaled <= std::numeric_limits<uint16_t>::max());
                frame = static_cast<uint16_t>(scaled);
            }

            WriteValue(stream, frame);
        }
    }
} // namespace

namespace xanim
{
    void DumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetXAnim::Type>& asset)
    {
        const auto* parts = asset.Asset();
        const auto assetFile = context.OpenAssetFile(GetFileNameForAssetName(asset.m_name));
        if (!assetFile)
            return;

        const auto numLoopFrames = GetNumLoopFrames(*parts);
        const auto useByteIndices = UseByteIndices(*parts);
        const auto boneTracks = ReconstructBoneTracks(asset);
        const auto deltaTrack = ReconstructDeltaTrack(*parts);
        std::vector<EncodedQuatTrack> encodedBoneQuats;
        encodedBoneQuats.reserve(boneTracks.size());
        for (const auto& bone : boneTracks)
            encodedBoneQuats.push_back(EncodeQuatTrack(bone.quat));

        auto& stream = *assetFile;

        const auto flags = static_cast<uint8_t>((parts->bLoop ? 1u : 0u) | (parts->bDelta ? 2u : 0u));
        const auto boneCount = static_cast<uint16_t>(parts->boneCount[9]);
        const auto assetType = static_cast<uint8_t>(static_cast<unsigned char>(parts->assetType));
        const auto framerate = static_cast<uint16_t>(std::lround(parts->framerate));

        WriteValue(stream, RAW_VERSION);
        // Looped raws store numframes directly; non-looped raws store numframes + 1.
        WriteValue(stream, static_cast<uint16_t>(parts->bLoop ? parts->numframes : numLoopFrames));
        WriteValue(stream, boneCount);
        WriteValue(stream, flags);
        WriteValue(stream, assetType);
        WriteValue(stream, framerate);

        if (parts->bDelta)
            WriteDeltaTrack(stream, deltaTrack, numLoopFrames, useByteIndices);

        if (!boneTracks.empty())
        {
            const auto bitmaskSize = (boneTracks.size() + 7uz) / 8uz;
            std::vector<uint8_t> flipQuat(bitmaskSize, 0);
            std::vector<uint8_t> simpleQuat(bitmaskSize, 0);

            for (auto i = 0uz; i < boneTracks.size(); i++)
            {
                if (encodedBoneQuats[i].flipQuat)
                    flipQuat[i >> 3u] |= static_cast<uint8_t>(1u << (i & 7u));

                if (QuatTypeUsesSimpleMask(boneTracks[i].quat.type))
                    simpleQuat[i >> 3u] |= static_cast<uint8_t>(1u << (i & 7u));
            }

            stream.write(reinterpret_cast<const char*>(flipQuat.data()), static_cast<std::streamsize>(flipQuat.size()));
            stream.write(reinterpret_cast<const char*>(simpleQuat.data()), static_cast<std::streamsize>(simpleQuat.size()));

            for (const auto& bone : boneTracks)
                WriteCString(stream, bone.name);

            for (auto i = 0uz; i < boneTracks.size(); i++)
            {
                WriteQuatTrack(stream, boneTracks[i].quat, encodedBoneQuats[i], numLoopFrames, useByteIndices);
                WriteTransTrack(stream, boneTracks[i].trans, numLoopFrames, useByteIndices);
            }
        }

        WriteNotifyTail(stream, asset);
    }
} // namespace xanim
