#include "CompiledXAnimWriter.h"

#include "Utils/Alignment.h"
#include "Utils/Logging/Log.h"
#include "Utils/StreamUtils.h"

#include <cassert>
#include <cmath>

using namespace xanim;

namespace
{
    // The linker decodes raw trans size[] with these exact float literals.
    // They correspond to 1.0f / 255.0f and 1.0f / 65535.0f, but we keep the
    // decompiled values to preserve binary-stable round trips.
    constexpr auto HALF_TRANS_SIZE_SCALE = 0.003921568859368563f;
    constexpr auto FULL_TRANS_SIZE_SCALE = 0.00001525902189314365f;

    class EncodedQuatTrack
    {
    public:
        bool m_flip_quat = false;
        std::vector<int16_t> m_stored_values;
    };

    uint8_t GetFlagsForVersion(const CompiledXAnimVersion version, const CommonXAnimParts& parts)
    {
        uint8_t flags = 0;

        const auto hasDelta3D = parts.m_delta_track && parts.m_delta_track->m_quat && parts.m_delta_track->m_quat->Is3DTrack();
        switch (version)
        {
        case CompiledXAnimVersion::VERSION_17:
            if (parts.m_looped)
                flags |= binary17::FLAG_LOOPED;
            if (parts.m_delta_track)
                flags |= binary17::FLAG_DELTA;
            break;

        case CompiledXAnimVersion::VERSION_18:
            if (parts.m_looped)
                flags |= binary18::FLAG_LOOPED;
            if (parts.m_delta_track)
                flags |= hasDelta3D ? binary18::FLAG_DELTA_3D : binary18::FLAG_DELTA;
            break;

        case CompiledXAnimVersion::VERSION_19:
        {
            const auto requiresT6Compatibility = hasDelta3D;

            if (requiresT6Compatibility)
                flags |= binary19::FLAG_T6_COMPATIBILITY;
            if (parts.m_looped)
                flags |= binary19::FLAG_LOOPED;
            if (parts.m_delta_track)
                flags |= hasDelta3D ? binary19::FLAG_T6_DELTA_3D : binary19::FLAG_DELTA;
            if (parts.m_left_hand_grip_ik)
                flags |= requiresT6Compatibility ? binary19::FLAG_T6_LEFT_HAND_GRIP_IK : binary19::FLAG_T5_LEFT_HAND_GRIP_IK;
            if (parts.m_streamable && !requiresT6Compatibility)
                flags |= binary19::FLAG_T5_STREAMABLE;
        }
        break;
        }

        return flags;
    }

    [[nodiscard]] uint16_t GetNumLoopFrames(const CommonXAnimParts& parts)
    {
        assert(parts.m_num_frames < std::numeric_limits<uint16_t>::max());

        // Raw non-looped xanims store numframes + 1 in keyed track counts/header fields.
        return static_cast<uint16_t>(parts.m_num_frames + 1u);
    }

    [[nodiscard]] bool QuatTypeUsesHalf(const QuatType type)
    {
        return type == QuatType::NO_QUAT || type == QuatType::HALF_QUAT || type == QuatType::HALF_QUAT_NO_SIZE;
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

    template<typename T>
    concept XQuatOrXQuat2 = std::is_array_v<decltype(T::value)> && std::is_integral_v<std::remove_extent_t<decltype(T::value)>>;

    template<XQuatOrXQuat2 T> [[nodiscard]] int64_t ComputeQuatDot(const T& lhs, const T& rhs)
    {
        int64_t result = 0;
        for (auto i = 0uz; i < std::extent_v<decltype(lhs.value)>; i++)
            result += static_cast<int64_t>(lhs.value[i]) * static_cast<int64_t>(rhs.value[i]);

        return result;
    }

    template<XQuatOrXQuat2 T> [[nodiscard]] EncodedQuatTrack EncodeQuatFrames(const std::vector<T>& frames, const bool allowFlipQuat)
    {
        constexpr auto COMPONENT_COUNT = std::extent_v<decltype(T::value)>;
        constexpr auto STORED_COMPONENT_COUNT = COMPONENT_COUNT - 1;

        EncodedQuatTrack result;
        if (frames.empty())
            return result;

        const auto frameCount = frames.size();

        // Raw IW3 xanims store only N-1 quat components. The loader reconstructs the
        // final component with a positive sqrt, applies the per-bone flip bit, and then
        // continuity-corrects subsequent frames by optionally negating whole quats.
        result.m_stored_values.reserve(frameCount * STORED_COMPONENT_COUNT);

        result.m_flip_quat = allowFlipQuat && frames[0].value[COMPONENT_COUNT - 1] < 0;
        const auto targetNegativeOmitted = result.m_flip_quat;

        for (size_t frameIndex = 0; frameIndex < frameCount; frameIndex++)
        {
            const auto& frame = frames[frameIndex];
            const auto omittedNegative = frame.value[COMPONENT_COUNT - 1] < 0;

            auto continuityNegated = false;
            if (frameIndex > 0u && omittedNegative != targetNegativeOmitted)
            {
                const auto& prevFrame = frames[(frameIndex - 1u)];
                continuityNegated = ComputeQuatDot(prevFrame, frame) > 0;
            }

            const auto rawNegated = result.m_flip_quat != continuityNegated;
            const auto sign = rawNegated ? -1 : 1;

            for (size_t componentIndex = 0; componentIndex < STORED_COMPONENT_COUNT; componentIndex++)
            {
                const auto value = static_cast<int>(frame.value[componentIndex]) * sign;
                assert(value >= std::numeric_limits<int16_t>::min() && value <= std::numeric_limits<int16_t>::max());
                result.m_stored_values.emplace_back(static_cast<int16_t>(value));
            }
        }

        return result;
    }

    [[nodiscard]] EncodedQuatTrack EncodeQuatTrack(const QuatTrack& quat)
    {
        switch (quat.m_type)
        {
        case QuatType::NO_QUAT:
            return {};

        case QuatType::HALF_QUAT_NO_SIZE:
            assert(quat.m_frames2.size() == 1);
            return EncodeQuatFrames(quat.m_frames2, true);

        case QuatType::FULL_QUAT_NO_SIZE:
            assert(quat.m_frames.size() == 1);
            return EncodeQuatFrames(quat.m_frames, true);

        case QuatType::HALF_QUAT:
            assert(quat.m_frames2.size() == quat.m_indices.size());
            return EncodeQuatFrames(quat.m_frames2, true);

        case QuatType::FULL_QUAT:
            assert(quat.m_frames.size() == quat.m_indices.size());
            return EncodeQuatFrames(quat.m_frames, true);
        }

        assert(false);
        return {};
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
                stream::WriteValue(stream, asByte);
            }
        }
        else
        {
            for (const auto index : indices)
                stream::WriteValue(stream, index);
        }
    }

    void WriteDeltaQuatTrack(std::ostream& stream, const CommonDeltaQuatTrack& quat, const uint16_t numLoopFrames, const bool useByteIndices)
    {
        const auto numQuatIndices = static_cast<uint16_t>(quat.m_frames.size());
        assert(numQuatIndices > 0);

        stream::WriteValue(stream, numQuatIndices);

        const auto encodedDeltaQuatFrames = EncodeQuatFrames(quat.m_frames, false);

        if (numQuatIndices == 1)
        {
            assert(encodedDeltaQuatFrames.m_stored_values.size() == 3);
            stream::WriteValue(stream, encodedDeltaQuatFrames.m_stored_values[0]);
            stream::WriteValue(stream, encodedDeltaQuatFrames.m_stored_values[1]);
            stream::WriteValue(stream, encodedDeltaQuatFrames.m_stored_values[2]);
        }
        else
        {
            assert(numQuatIndices > 1u);
            assert(quat.m_indices.size() == numQuatIndices);
            assert(encodedDeltaQuatFrames.m_stored_values.size() == numQuatIndices * 3);

            WriteIndicesIfNeeded(stream, quat.m_indices, numLoopFrames, useByteIndices);
            for (const auto value : encodedDeltaQuatFrames.m_stored_values)
                stream::WriteValue(stream, value);
        }
    }

    void WriteDeltaQuat2Track(std::ostream& stream, const CommonXAnimDeltaTrack& delta, const uint16_t numLoopFrames, const bool useByteIndices)
    {
        if (!delta.m_quat)
        {
            stream::WriteValue(stream, static_cast<uint16_t>(0));
            return;
        }

        const auto numQuatIndices = static_cast<uint16_t>(delta.m_quat->m_frames2.size());
        assert(numQuatIndices > 0);

        stream::WriteValue(stream, numQuatIndices);

        const auto encodedDeltaQuatFrames = EncodeQuatFrames(delta.m_quat->m_frames2, false);

        if (numQuatIndices == 1)
        {
            assert(encodedDeltaQuatFrames.m_stored_values.size() == 1);
            stream::WriteValue(stream, encodedDeltaQuatFrames.m_stored_values[0]);
        }
        else
        {
            assert(numQuatIndices > 1u);
            assert(delta.m_quat->m_indices.size() == numQuatIndices);
            assert(encodedDeltaQuatFrames.m_stored_values.size() == numQuatIndices);

            WriteIndicesIfNeeded(stream, delta.m_quat->m_indices, numLoopFrames, useByteIndices);
            for (const auto value : encodedDeltaQuatFrames.m_stored_values)
                stream::WriteValue(stream, value);
        }
    }

    [[nodiscard]] float EncodeRawTransSize(const float value, const bool smallTrans)
    {
        const auto scale = smallTrans ? HALF_TRANS_SIZE_SCALE : FULL_TRANS_SIZE_SCALE;
        return value / scale;
    }

    void WriteDeltaTransTrack(std::ostream& stream, const CommonXAnimDeltaTrack& delta, const uint16_t numLoopFrames, const bool useByteIndices)
    {
        if (!delta.m_trans)
        {
            stream::WriteValue(stream, static_cast<uint16_t>(0));
            return;
        }

        if (delta.m_trans->m_constant)
        {
            stream::WriteValue(stream, static_cast<uint16_t>(1));
            for (const auto value : *delta.m_trans->m_constant)
                stream::WriteValue(stream, value);
            return;
        }

        const auto numTransIndices = static_cast<uint16_t>(delta.m_trans->m_indices.size());
        assert(numTransIndices > 1);

        stream::WriteValue(stream, numTransIndices);
        WriteIndicesIfNeeded(stream, delta.m_trans->m_indices, numLoopFrames, useByteIndices);

        const auto smallTrans = !delta.m_trans->m_frames_u8.empty();
        stream::WriteValue(stream, static_cast<uint8_t>(smallTrans ? 1 : 0));
        for (const auto value : delta.m_trans->m_mins)
            stream::WriteValue(stream, value);

        if (smallTrans)
        {
            assert(delta.m_trans->m_frames_u8.size() == numTransIndices);
            for (const auto value : delta.m_trans->m_size)
                stream::WriteValue(stream, EncodeRawTransSize(value, true));

            for (const auto vec3U8 : delta.m_trans->m_frames_u8)
            {
                stream::WriteValue(stream, vec3U8.value[0]);
                stream::WriteValue(stream, vec3U8.value[1]);
                stream::WriteValue(stream, vec3U8.value[2]);
            }
        }
        else
        {
            assert(delta.m_trans->m_frames_u16.size() == numTransIndices);
            for (const auto value : delta.m_trans->m_size)
                stream::WriteValue(stream, EncodeRawTransSize(value, false));

            for (const auto vec3U16 : delta.m_trans->m_frames_u16)
            {
                stream::WriteValue(stream, vec3U16.value[0]);
                stream::WriteValue(stream, vec3U16.value[1]);
                stream::WriteValue(stream, vec3U16.value[2]);
            }
        }
    }

    void WriteDeltaTrack(std::ostream& stream, const CommonXAnimDeltaTrack& delta, const uint16_t numLoopFrames, const bool useByteIndices)
    {
        if (delta.m_quat && delta.m_quat->Is3DTrack())
            WriteDeltaQuatTrack(stream, *delta.m_quat, numLoopFrames, useByteIndices);
        else
            WriteDeltaQuat2Track(stream, delta, numLoopFrames, useByteIndices);
        WriteDeltaTransTrack(stream, delta, numLoopFrames, useByteIndices);
    }

    void WriteQuatTrack(
        std::ostream& stream, const QuatTrack& quat, const EncodedQuatTrack& encodedQuat, const uint16_t numLoopFrames, const bool useByteIndices)
    {
        switch (quat.m_type)
        {
        case QuatType::NO_QUAT:
        {
            stream::WriteValue(stream, static_cast<uint16_t>(0));
            break;
        }

        case QuatType::HALF_QUAT_NO_SIZE:
        {
            assert(encodedQuat.m_stored_values.size() == 1uz);
            stream::WriteValue(stream, static_cast<uint16_t>(1));
            stream::WriteValue(stream, encodedQuat.m_stored_values[0]);
            break;
        }

        case QuatType::FULL_QUAT_NO_SIZE:
        {
            assert(encodedQuat.m_stored_values.size() == 3uz);
            stream::WriteValue(stream, static_cast<uint16_t>(1));
            for (const auto value : encodedQuat.m_stored_values)
                stream::WriteValue(stream, value);
            break;
        }

        case QuatType::HALF_QUAT:
        {
            const auto frameCount = quat.m_indices.size();
            assert(frameCount > 0uz);
            assert(quat.m_frames2.size() == frameCount);
            assert(encodedQuat.m_stored_values.size() == frameCount);

            stream::WriteValue(stream, static_cast<uint16_t>(frameCount));
            WriteIndicesIfNeeded(stream, quat.m_indices, numLoopFrames, useByteIndices);
            for (const auto value : encodedQuat.m_stored_values)
                stream::WriteValue(stream, value);
            break;
        }

        case QuatType::FULL_QUAT:
        {
            const auto frameCount = quat.m_indices.size();
            assert(frameCount > 0uz);
            assert(quat.m_frames.size() == frameCount);
            assert(encodedQuat.m_stored_values.size() == frameCount * 3uz);

            stream::WriteValue(stream, static_cast<uint16_t>(frameCount));
            WriteIndicesIfNeeded(stream, quat.m_indices, numLoopFrames, useByteIndices);
            for (const auto value : encodedQuat.m_stored_values)
                stream::WriteValue(stream, value);
            break;
        }
        }
    }

    void WriteTransTrack(std::ostream& stream, const TransTrack& trans, const uint16_t numLoopFrames, const bool useByteIndices)
    {
        switch (trans.m_type)
        {
        case TransType::NO_TRANS:
        {
            stream::WriteValue(stream, static_cast<uint16_t>(0));
            break;
        }

        case TransType::TRANS_NO_SIZE:
        {
            stream::WriteValue(stream, static_cast<uint16_t>(1));
            for (const auto value : trans.m_constant)
                stream::WriteValue(stream, value);
            break;
        }

        case TransType::SMALL_TRANS:
        {
            const auto frameCount = trans.m_indices.size();
            assert(frameCount > 0uz);
            assert(trans.m_byte_frames.size() == frameCount * 3uz);

            stream::WriteValue(stream, static_cast<uint16_t>(frameCount));
            WriteIndicesIfNeeded(stream, trans.m_indices, numLoopFrames, useByteIndices);

            constexpr auto smallTrans = static_cast<uint8_t>(1);
            stream::WriteValue(stream, smallTrans);

            for (const auto value : trans.m_mins)
                stream::WriteValue(stream, value);
            for (const auto value : trans.m_size)
                stream::WriteValue(stream, EncodeRawTransSize(value, true));

            stream::Write(stream, trans.m_byte_frames.data(), trans.m_byte_frames.size());
            break;
        }

        case TransType::FULL_TRANS:
        {
            const auto frameCount = trans.m_indices.size();
            assert(frameCount > 0uz);
            assert(trans.m_short_frames.size() == frameCount * 3uz);

            stream::WriteValue(stream, static_cast<uint16_t>(frameCount));
            WriteIndicesIfNeeded(stream, trans.m_indices, numLoopFrames, useByteIndices);

            constexpr auto smallTrans = static_cast<uint8_t>(0);
            stream::WriteValue(stream, smallTrans);

            for (const auto value : trans.m_mins)
                stream::WriteValue(stream, value);
            for (const auto value : trans.m_size)
                stream::WriteValue(stream, EncodeRawTransSize(value, false));

            for (const auto value : trans.m_short_frames)
                stream::WriteValue(stream, value);
            break;
        }
        }
    }

    void WriteNoteTracks(std::ostream& stream, const CommonXAnimParts& parts)
    {
        const auto notifyCount = parts.m_notifies.size();

        auto rawNotifyCount = notifyCount;
        if (notifyCount > 0uz)
        {
            const auto& lastNotify = parts.m_notifies[notifyCount - 1];

            // The linker appends a synthetic "end" notify at 1.0f to the loaded asset state.
            if (lastNotify.m_name == "end" && std::abs(lastNotify.m_time - 1.0f) < 0.0001f)
                rawNotifyCount--;
        }

        assert(rawNotifyCount < 255uz);
        const auto rawNotifyCountByte = static_cast<uint8_t>(rawNotifyCount);
        stream::WriteValue(stream, rawNotifyCountByte);

        for (auto i = 0uz; i < rawNotifyCount; i++)
        {
            const auto& notify = parts.m_notifies[i];
            stream::WriteCString(stream, notify.m_name);

            uint16_t frame = 0;
            if (parts.m_num_frames > 0)
            {
                const auto scaled = static_cast<long>(std::lround(notify.m_time * static_cast<float>(parts.m_num_frames)));
                assert(scaled >= 0 && scaled <= std::numeric_limits<uint16_t>::max());
                frame = static_cast<uint16_t>(scaled);
            }

            stream::WriteValue(stream, frame);
        }
    }
} // namespace

namespace xanim
{
    void WriteCompiledXAnim(std::ostream& stream, const CommonXAnimParts& parts, CompiledXAnimVersion version)
    {
        const auto numLoopFrames = GetNumLoopFrames(parts);
        const auto useByteIndices = parts.m_num_frames < 256;

        std::vector<EncodedQuatTrack> encodedBoneQuats;
        encodedBoneQuats.reserve(parts.m_bone_tracks.size());
        for (const auto& bone : parts.m_bone_tracks)
            encodedBoneQuats.emplace_back(EncodeQuatTrack(bone.m_quat));

        const auto flags = GetFlagsForVersion(version, parts);
        const auto boneCount = static_cast<uint16_t>(parts.m_bone_tracks.size());
        const auto assetType = static_cast<uint8_t>(parts.m_asset_type);
        const auto framerate = static_cast<uint16_t>(std::lround(parts.m_frame_rate));

        stream::WriteValue(stream, static_cast<uint16_t>(version));
        // Looped raws store numframes directly; non-looped raws store numframes + 1.
        stream::WriteValue(stream, static_cast<uint16_t>(parts.m_looped ? parts.m_num_frames : numLoopFrames));
        stream::WriteValue(stream, boneCount);
        stream::WriteValue(stream, flags);
        stream::WriteValue(stream, assetType);
        stream::WriteValue(stream, framerate);
        if (version == CompiledXAnimVersion::VERSION_19 && parts.m_streamable && (flags & binary19::FLAG_T6_COMPATIBILITY) == 0)
            stream::WriteValue(stream, parts.m_primed_length);

        if (parts.m_delta_track)
            WriteDeltaTrack(stream, *parts.m_delta_track, numLoopFrames, useByteIndices);

        if (!parts.m_bone_tracks.empty())
        {
            const auto bitmaskSize = utils::Align<size_t>(boneCount, 8u) / 8u;
            std::vector<uint8_t> flipQuat(bitmaskSize, 0);
            std::vector<uint8_t> halfQuat(bitmaskSize, 0);

            for (auto i = 0u; i < boneCount; i++)
            {
                if (encodedBoneQuats[i].m_flip_quat)
                    flipQuat[i / 8u] |= static_cast<uint8_t>(1u << (i % 8u));

                if (QuatTypeUsesHalf(parts.m_bone_tracks[i].m_quat.m_type))
                    halfQuat[i / 8u] |= static_cast<uint8_t>(1u << (i % 8u));
            }

            stream::Write(stream, flipQuat.data(), flipQuat.size());
            stream::Write(stream, halfQuat.data(), halfQuat.size());

            for (const auto& bone : parts.m_bone_tracks)
                stream::WriteCString(stream, bone.m_name);

            for (auto i = 0u; i < boneCount; i++)
            {
                WriteQuatTrack(stream, parts.m_bone_tracks[i].m_quat, encodedBoneQuats[i], numLoopFrames, useByteIndices);
                WriteTransTrack(stream, parts.m_bone_tracks[i].m_trans, numLoopFrames, useByteIndices);
            }
        }

        WriteNoteTracks(stream, parts);
    }
} // namespace xanim
