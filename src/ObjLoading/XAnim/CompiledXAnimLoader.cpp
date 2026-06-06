#include "CompiledXAnimLoader.h"

#include "Utils/Alignment.h"
#include "Utils/StreamUtils.h"
#include "XAnim/BinaryXAnimCommon.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <format>
#include <numeric>

using namespace xanim;

namespace
{
    // The linker decodes raw trans size[] with these exact float literals.
    // They correspond to 1.0f / 255.0f and 1.0f / 65535.0f, but we keep the
    // decompiled values to preserve binary-stable round trips.
    constexpr auto HALF_TRANS_SIZE_SCALE = 0.003921568859368563f;
    constexpr auto FULL_TRANS_SIZE_SCALE = 0.00001525902189314365f;

    std::expected<CompiledXAnimVersion, std::string> IdentifyVersion(std::istream& stream)
    {
        const auto fileVersion = stream::ReadValue<uint16_t>(stream);
        switch (static_cast<CompiledXAnimVersion>(fileVersion))
        {
        case CompiledXAnimVersion::VERSION_17:
            return CompiledXAnimVersion::VERSION_17;
        case CompiledXAnimVersion::VERSION_18:
            return CompiledXAnimVersion::VERSION_18;
        case CompiledXAnimVersion::VERSION_19:
            return CompiledXAnimVersion::VERSION_19;
        }

        return std::unexpected(std::format("Version {} is not supported", fileVersion));
    }

    CommonXQuat ConsumeQuat(std::istream& stream)
    {
        CommonXQuat quat{};
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

        return quat;
    }

    CommonXQuat2 ConsumeQuat2(std::istream& stream)
    {
        CommonXQuat2 quat2{};
        quat2.value[0] = stream::ReadValue<int16_t>(stream);

        int32_t temp = 0x3FFF0001 - quat2.value[0] * quat2.value[0];
        if (temp <= 0)
            temp = 0;
        else
            temp = static_cast<int32_t>(floor(std::sqrt(static_cast<float>(temp)) + 0.5f));

        assert(temp >= std::numeric_limits<int16_t>::min() && temp <= std::numeric_limits<int16_t>::max());
        quat2.value[1] = static_cast<int16_t>(temp);

        return quat2;
    }

    void FlipQuat(CommonXQuat& quat)
    {
        quat.value[0] = static_cast<int16_t>(-quat.value[0]);
        quat.value[1] = static_cast<int16_t>(-quat.value[1]);
        quat.value[2] = static_cast<int16_t>(-quat.value[2]);
        quat.value[3] = static_cast<int16_t>(-quat.value[3]);
    }

    void FlipQuat2(CommonXQuat2& quat)
    {
        quat.value[0] = static_cast<int16_t>(-quat.value[0]);
        quat.value[1] = static_cast<int16_t>(-quat.value[1]);
    }

    [[nodiscard]] float DecodeRawTransSize(const float value, const bool smallTrans)
    {
        const auto scale = smallTrans ? HALF_TRANS_SIZE_SCALE : FULL_TRANS_SIZE_SCALE;
        return value * scale;
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

    std::expected<std::optional<CommonDeltaQuatTrack>, std::string>
        LoadDeltaQuatTrack(std::istream& stream, const bool useByteIndices, const uint16_t numLoopFrames)
    {
        const auto numQuatIndices = stream::ReadValue<uint16_t>(stream);
        if (numQuatIndices == 0)
            return std::nullopt;

        CommonDeltaQuatTrack deltaQuatTrack;
        if (numQuatIndices == 1)
        {
            deltaQuatTrack.m_frames.emplace_back(ConsumeQuat(stream));
            return deltaQuatTrack;
        }

        LoadIndicesIfNeeded(stream, deltaQuatTrack.m_indices, numQuatIndices, useByteIndices, numLoopFrames);

        deltaQuatTrack.m_frames.reserve(numQuatIndices);
        for (auto quatIndexNum = 0u; quatIndexNum < numQuatIndices; ++quatIndexNum)
        {
            auto& curFrame = deltaQuatTrack.m_frames.emplace_back(ConsumeQuat(stream));

            if (quatIndexNum > 0)
            {
                const auto& prevFrame = deltaQuatTrack.m_frames[quatIndexNum - 1];
                const auto dot = prevFrame.value[0] * curFrame.value[0] + prevFrame.value[1] * curFrame.value[1] + prevFrame.value[2] * curFrame.value[2]
                                 + prevFrame.value[3] * curFrame.value[3];
                if (dot < 0)
                    FlipQuat(curFrame);
            }
        }

        return deltaQuatTrack;
    }

    std::expected<std::optional<CommonDeltaQuatTrack>, std::string>
        LoadDeltaQuat2Track(std::istream& stream, const bool useByteIndices, const uint16_t numLoopFrames)
    {
        const auto numQuatIndices = stream::ReadValue<uint16_t>(stream);
        if (numQuatIndices == 0)
            return std::nullopt;

        CommonDeltaQuatTrack deltaQuatTrack;
        if (numQuatIndices == 1)
        {
            deltaQuatTrack.m_frames2.emplace_back(ConsumeQuat2(stream));
            return deltaQuatTrack;
        }

        LoadIndicesIfNeeded(stream, deltaQuatTrack.m_indices, numQuatIndices, useByteIndices, numLoopFrames);

        deltaQuatTrack.m_frames2.reserve(numQuatIndices);
        for (auto quatIndexNum = 0u; quatIndexNum < numQuatIndices; ++quatIndexNum)
        {
            auto& curFrame = deltaQuatTrack.m_frames2.emplace_back(ConsumeQuat2(stream));

            if (quatIndexNum > 0)
            {
                const auto& prevFrame = deltaQuatTrack.m_frames2[quatIndexNum - 1];
                if (prevFrame.value[0] * curFrame.value[0] + prevFrame.value[1] * curFrame.value[1] < 0)
                    FlipQuat2(curFrame);
            }
        }

        return deltaQuatTrack;
    }

    std::expected<std::optional<CommonDeltaTransTrack>, std::string>
        LoadDeltaTransTrack(std::istream& stream, const bool useByteIndices, const uint16_t numLoopFrames)
    {
        const auto numTransIndices = stream::ReadValue<uint16_t>(stream);
        if (numTransIndices == 0)
            return std::nullopt;

        CommonDeltaTransTrack deltaTransTrack;
        if (numTransIndices == 1)
        {
            deltaTransTrack.m_constant.emplace(std::array<float, 3>({
                stream::ReadValue<float>(stream),
                stream::ReadValue<float>(stream),
                stream::ReadValue<float>(stream),
            }));
            return deltaTransTrack;
        }

        LoadIndicesIfNeeded(stream, deltaTransTrack.m_indices, numTransIndices, useByteIndices, numLoopFrames);

        const auto smallTrans = stream::ReadValue<bool>(stream);

        deltaTransTrack.m_mins[0] = stream::ReadValue<float>(stream);
        deltaTransTrack.m_mins[1] = stream::ReadValue<float>(stream);
        deltaTransTrack.m_mins[2] = stream::ReadValue<float>(stream);

        deltaTransTrack.m_size[0] = DecodeRawTransSize(stream::ReadValue<float>(stream), smallTrans);
        deltaTransTrack.m_size[1] = DecodeRawTransSize(stream::ReadValue<float>(stream), smallTrans);
        deltaTransTrack.m_size[2] = DecodeRawTransSize(stream::ReadValue<float>(stream), smallTrans);

        if (smallTrans)
        {
            deltaTransTrack.m_frames_u8.resize(numTransIndices);
            stream::Read(stream, deltaTransTrack.m_frames_u8.data(), numTransIndices * sizeof(CommonVec3U8));
        }
        else
        {
            deltaTransTrack.m_frames_u16.resize(numTransIndices);
            stream::Read(stream, deltaTransTrack.m_frames_u16.data(), numTransIndices * sizeof(CommonVec3U16));
        }

        return deltaTransTrack;
    }

    std::expected<std::unique_ptr<CommonXAnimDeltaTrack>, std::string>
        LoadDeltaTrack(std::istream& stream, const bool hasDelta3D, const bool useByteIndices, const uint16_t numLoopFrames)
    {
        auto delta = std::make_unique<CommonXAnimDeltaTrack>();

        auto maybeLoadedDeltaQuat =
            hasDelta3D ? LoadDeltaQuatTrack(stream, useByteIndices, numLoopFrames) : LoadDeltaQuat2Track(stream, useByteIndices, numLoopFrames);
        if (!maybeLoadedDeltaQuat.has_value())
            return std::unexpected(std::move(maybeLoadedDeltaQuat).error());
        delta->m_quat = std::move(maybeLoadedDeltaQuat).value();

        auto maybeLoadedDeltaTrans = LoadDeltaTransTrack(stream, useByteIndices, numLoopFrames);
        if (!maybeLoadedDeltaTrans.has_value())
            return std::unexpected(std::move(maybeLoadedDeltaTrans).error());
        delta->m_trans = std::move(maybeLoadedDeltaTrans).value();

        return delta;
    }

    QuatTrack ReadQuatTrack(std::istream& stream, const uint16_t numLoopFrames, const bool useByteIndices, const bool flipQuat, const bool halfQuat)
    {
        QuatTrack quatTrack;

        const auto numQuatIndices = stream::ReadValue<uint16_t>(stream);
        if (numQuatIndices == 0)
        {
            assert(halfQuat);
            quatTrack.m_type = QuatType::NO_QUAT;
            return quatTrack;
        }

        if (numQuatIndices == 1)
        {
            quatTrack.m_type = halfQuat ? QuatType::HALF_QUAT_NO_SIZE : QuatType::FULL_QUAT_NO_SIZE;
            if (halfQuat)
            {
                auto quat2 = ConsumeQuat2(stream);
                if (flipQuat)
                    FlipQuat2(quat2);

                quatTrack.m_frames2.emplace_back(quat2);
            }
            else
            {
                auto quat = ConsumeQuat(stream);
                if (flipQuat)
                    FlipQuat(quat);

                quatTrack.m_frames.emplace_back(quat);
            }

            return quatTrack;
        }

        LoadIndicesIfNeeded(stream, quatTrack.m_indices, numQuatIndices, useByteIndices, numLoopFrames);

        if (halfQuat)
        {
            quatTrack.m_type = QuatType::HALF_QUAT;
            quatTrack.m_frames2.reserve(numQuatIndices);
            for (auto quatIndexNum = 0u; quatIndexNum < numQuatIndices; quatIndexNum++)
            {
                auto& curFrame = quatTrack.m_frames2.emplace_back(ConsumeQuat2(stream));

                if (quatIndexNum > 0)
                {
                    const auto& prevFrame = quatTrack.m_frames2[quatIndexNum - 1];
                    if (prevFrame.value[0] * curFrame.value[0] + prevFrame.value[1] * curFrame.value[1] < 0)
                        FlipQuat2(curFrame);
                }
                else if (flipQuat)
                    FlipQuat2(curFrame);
            }
        }
        else
        {
            quatTrack.m_type = QuatType::FULL_QUAT;
            quatTrack.m_frames.reserve(numQuatIndices);
            for (auto quatIndexNum = 0u; quatIndexNum < numQuatIndices; quatIndexNum++)
            {
                auto& curFrame = quatTrack.m_frames.emplace_back(ConsumeQuat(stream));

                if (quatIndexNum > 0)
                {
                    const auto& prevFrame = quatTrack.m_frames[quatIndexNum - 1];
                    const auto dot = prevFrame.value[0] * curFrame.value[0] + prevFrame.value[1] * curFrame.value[1] + prevFrame.value[2] * curFrame.value[2]
                                     + prevFrame.value[3] * curFrame.value[3];
                    if (dot < 0)
                        FlipQuat(curFrame);
                }
                else if (flipQuat)
                    FlipQuat(curFrame);
            }
        }

        return quatTrack;
    }

    TransTrack ReadTransTrack(std::istream& stream, const uint16_t numLoopFrames, const bool useByteIndices)
    {
        TransTrack transTrack;

        const auto numTransIndices = stream::ReadValue<uint16_t>(stream);
        if (numTransIndices == 0)
        {
            transTrack.m_type = TransType::NO_TRANS;
            return transTrack;
        }

        if (numTransIndices == 1)
        {
            transTrack.m_type = TransType::TRANS_NO_SIZE;
            for (auto& value : transTrack.m_constant)
                value = stream::ReadValue<float>(stream);
            return transTrack;
        }

        LoadIndicesIfNeeded(stream, transTrack.m_indices, numTransIndices, useByteIndices, numLoopFrames);

        const auto smallTrans = stream::ReadValue<bool>(stream);
        transTrack.m_type = smallTrans ? TransType::SMALL_TRANS : TransType::FULL_TRANS;

        for (auto& value : transTrack.m_mins)
            value = stream::ReadValue<float>(stream);
        for (auto& value : transTrack.m_size)
            value = DecodeRawTransSize(stream::ReadValue<float>(stream), smallTrans);

        if (smallTrans)
        {
            static_assert(sizeof(decltype(transTrack.m_frames_u8)::value_type) == sizeof(uint8_t) * 3u);
            transTrack.m_frames_u8.resize(numTransIndices);
            stream::Read(stream, transTrack.m_frames_u8.data(), numTransIndices * sizeof(uint8_t) * 3);
        }
        else
        {
            static_assert(sizeof(decltype(transTrack.m_frames_u16)::value_type) == sizeof(int16_t) * 3u);
            transTrack.m_frames_u16.resize(numTransIndices);
            stream::Read(stream, transTrack.m_frames_u16.data(), numTransIndices * sizeof(uint16_t) * 3);
        }

        return transTrack;
    }

    void ReadNoteTracks(std::istream& stream, CommonXAnimParts& parts)
    {
        const auto numNoteTracks = stream::ReadValue<uint8_t>(stream);

        parts.m_notifies.reserve(numNoteTracks + 1);
        for (auto notifyIndex = 0u; notifyIndex < numNoteTracks; notifyIndex++)
        {
            const auto notifyName = stream::ReadCString(stream);

            const auto frame = stream::ReadValue<uint16_t>(stream);
            const auto time = parts.m_num_frames > 0 ? static_cast<float>(frame) / static_cast<float>(parts.m_num_frames) : 0;
            assert(time >= 0.0f && time <= 1.0f);
            parts.m_notifies.emplace_back(notifyName, time);
        }

        // This notify is always automatically added
        parts.m_notifies.emplace_back("end", 1.0f);
    }

    bool IsLooped(const uint8_t flags, const CompiledXAnimVersion version)
    {
        switch (version)
        {
        case CompiledXAnimVersion::VERSION_17:
            return (flags & binary17::FLAG_LOOPED) > 0;
        case CompiledXAnimVersion::VERSION_18:
            return (flags & binary18::FLAG_LOOPED) > 0;
        case CompiledXAnimVersion::VERSION_19:
            return (flags & binary19::FLAG_LOOPED) > 0;
        }

        return false;
    }

    bool HasDelta(const uint8_t flags, const CompiledXAnimVersion version)
    {
        switch (version)
        {
        case CompiledXAnimVersion::VERSION_17:
            return (flags & binary17::FLAG_DELTA) > 0;
        case CompiledXAnimVersion::VERSION_18:
            return (flags & binary18::FLAG_DELTA) > 0;
        case CompiledXAnimVersion::VERSION_19:
            return (flags & binary19::FLAG_DELTA) > 0;
        }

        return false;
    }

    bool HasDelta3D(const uint8_t flags, const CompiledXAnimVersion version)
    {
        switch (version)
        {
        case CompiledXAnimVersion::VERSION_18:
            return (flags & binary18::FLAG_DELTA_3D) > 0;
        case CompiledXAnimVersion::VERSION_19:
            return (flags & binary19::FLAG_T6_COMPATIBILITY) > 0 && (flags & binary19::FLAG_T6_DELTA_3D) > 0;
        case CompiledXAnimVersion::VERSION_17:
            return false;
        }

        return false;
    }

    bool IsLeftHandGripIk(const uint8_t flags, const CompiledXAnimVersion version)
    {
        switch (version)
        {
        case CompiledXAnimVersion::VERSION_19:
            if (flags & binary19::FLAG_T6_COMPATIBILITY)
                return (flags & binary19::FLAG_T6_LEFT_HAND_GRIP_IK) > 0;
            return (flags & binary19::FLAG_T5_LEFT_HAND_GRIP_IK) > 0;
        case CompiledXAnimVersion::VERSION_17:
        case CompiledXAnimVersion::VERSION_18:
            return false;
        }

        return false;
    }

    bool IsStreamable(const uint8_t flags, const CompiledXAnimVersion version)
    {
        switch (version)
        {
        case CompiledXAnimVersion::VERSION_19:
            if (flags & binary19::FLAG_T6_COMPATIBILITY)
                return false;
            return (flags & binary19::FLAG_T5_STREAMABLE) > 0;

        case CompiledXAnimVersion::VERSION_17:
        case CompiledXAnimVersion::VERSION_18:
            return false;
        }

        return false;
    }
} // namespace

namespace xanim
{
    std::expected<std::unique_ptr<CommonXAnimParts>, std::string> LoadCompiledXAnim(std::istream& stream)
    {
        auto maybeVersion = IdentifyVersion(stream);
        if (!maybeVersion)
            return std::unexpected(std::move(maybeVersion).error());

        auto parts = std::make_unique<CommonXAnimParts>();

        const auto version = maybeVersion.value();

        const auto numFrames = stream::ReadValue<uint16_t>(stream);
        const auto boneCount = stream::ReadValue<uint16_t>(stream);
        const auto flags = stream::ReadValue<uint8_t>(stream);
        const auto assetType = stream::ReadValue<uint8_t>(stream);
        const auto framerate = stream::ReadValue<uint16_t>(stream);
        if (stream.fail())
            return std::unexpected("Truncated file");

        const bool isLooped = IsLooped(flags, version);
        const bool hasDelta = HasDelta(flags, version);
        const bool hasDelta3D = HasDelta3D(flags, version);
        const bool leftHandGripIk = IsLeftHandGripIk(flags, version);
        const bool streamable = IsStreamable(flags, version);
        const uint16_t numLoopFrames = isLooped ? numFrames + 1u : numFrames;

        parts->m_num_frames = numLoopFrames - 1;
        parts->m_looped = isLooped;
        parts->m_left_hand_grip_ik = leftHandGripIk;
        parts->m_streamable = streamable;
        parts->m_asset_type = assetType;
        parts->m_frame_rate = static_cast<float>(framerate);

        if (version == CompiledXAnimVersion::VERSION_19 && streamable)
            parts->m_primed_length = stream::ReadValue<float>(stream);

        const auto useByteIndices = parts->m_num_frames < 256;

        if (hasDelta || hasDelta3D)
        {
            auto maybeBoneTrack = LoadDeltaTrack(stream, hasDelta3D, useByteIndices, numLoopFrames);
            if (!maybeBoneTrack.has_value())
                return std::unexpected(std::move(maybeBoneTrack).error());

            parts->m_delta_track = std::move(maybeBoneTrack).value();
        }

        if (boneCount > 0)
        {
            const auto bitmaskSize = utils::Align<size_t>(boneCount, 8u) / 8u;
            std::vector<uint8_t> flipQuatBits(bitmaskSize, 0);
            std::vector<uint8_t> halfQuatBits(bitmaskSize, 0);
            stream::Read(stream, flipQuatBits.data(), bitmaskSize);
            stream::Read(stream, halfQuatBits.data(), bitmaskSize);

            parts->m_bone_tracks.resize(boneCount);
            for (size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex)
                parts->m_bone_tracks[boneIndex].m_name = stream::ReadCString(stream);

            for (size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex)
            {
                auto& boneTrack = parts->m_bone_tracks[boneIndex];

                const bool flipQuat = flipQuatBits[boneIndex / 8u] & static_cast<uint8_t>(1u << (boneIndex % 8u));
                const bool halfQuat = halfQuatBits[boneIndex / 8u] & static_cast<uint8_t>(1u << (boneIndex % 8u));

                boneTrack.m_quat = ReadQuatTrack(stream, numLoopFrames, useByteIndices, flipQuat, halfQuat);
                boneTrack.m_trans = ReadTransTrack(stream, numLoopFrames, useByteIndices);
            }
        }
        parts->SortBoneTracksForQuats();

        ReadNoteTracks(stream, *parts);

        assert(stream.peek() == std::char_traits<char>::eof());
        return parts;
    }
} // namespace xanim
