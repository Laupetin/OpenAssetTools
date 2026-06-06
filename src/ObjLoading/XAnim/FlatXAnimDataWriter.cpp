#include "FlatXAnimDataWriter.h"

#include <cassert>
#include <iterator>

using namespace xanim;

namespace
{
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

    void WriteFloat3(FlatData& writeCursor, const std::array<float, 3>& value)
    {
        for (const float f : value)
            writeCursor.m_data_int.emplace_back(FloatBitsToInt(f));
    }

    void WritePackedIndices(FlatData& writeCursor, const std::vector<uint16_t>& indices, const bool useByteIndices)
    {
        const auto indexCount = indices.size();
        writeCursor.m_data_short.emplace_back(static_cast<int16_t>(indexCount - 1)); // storedSize

        if (useByteIndices)
        {
            for (const auto index : indices)
            {
                assert(index <= std::numeric_limits<uint8_t>::max());
                writeCursor.m_data_byte.emplace_back(static_cast<uint8_t>(index));
            }
        }
        else if (indexCount >= 65)
        {
            // The linker moves 16-bit frame indices into the top-level indices pool only when
            // the in-memory stored size is at least 64, i.e. frameCount >= 65.
            std::ranges::copy(indices, std::back_inserter(writeCursor.m_indices));

            // The game inserts checkpoint values in dataShort
            // Those checkpoint values are copied from positions in the full index list: the first entry, then every 256th entry, and always the final entry.
            // The final entry is included even when it does not land exactly on a 256-entry boundary.
            const auto longTableSize = ((indexCount - 2) / 256u) + 1;
            for (auto i = 0u; i < longTableSize; i++)
                writeCursor.m_data_short.emplace_back(indices[256 * i]);
            writeCursor.m_data_short.emplace_back(indices[indices.size() - 1]);
        }
        else
        {
            std::ranges::copy(indices, std::back_inserter(writeCursor.m_data_short));
        }
    }

    void ProcessQuatTrack(FlatData& writeCursor, const QuatTrack& quatTrack, const bool useByteIndices)
    {
        switch (quatTrack.m_type)
        {
        case QuatType::NO_QUAT:
            break;

        case QuatType::HALF_QUAT:
            WritePackedIndices(writeCursor, quatTrack.m_indices, useByteIndices);
            assert(quatTrack.m_frames2.size() == quatTrack.m_indices.size());

            writeCursor.m_random_data_short.reserve(writeCursor.m_random_data_short.size() + quatTrack.m_frames2.size() * 2);
            for (const auto& quat2 : quatTrack.m_frames2)
            {
                writeCursor.m_random_data_short.emplace_back(quat2.value[0]);
                writeCursor.m_random_data_short.emplace_back(quat2.value[1]);
            }
            break;

        case QuatType::FULL_QUAT:
            WritePackedIndices(writeCursor, quatTrack.m_indices, useByteIndices);
            assert(quatTrack.m_frames.size() == quatTrack.m_indices.size());

            writeCursor.m_random_data_short.reserve(writeCursor.m_random_data_short.size() + quatTrack.m_frames.size() * 4);
            for (const auto& quat : quatTrack.m_frames)
            {
                writeCursor.m_random_data_short.emplace_back(quat.value[0]);
                writeCursor.m_random_data_short.emplace_back(quat.value[1]);
                writeCursor.m_random_data_short.emplace_back(quat.value[2]);
                writeCursor.m_random_data_short.emplace_back(quat.value[3]);
            }
            break;

        case QuatType::HALF_QUAT_NO_SIZE:
        {
            assert(quatTrack.m_frames2.size() == 1);
            writeCursor.m_data_short.reserve(writeCursor.m_data_short.size() + 2);

            const auto& quat2 = quatTrack.m_frames2[0];
            writeCursor.m_data_short.emplace_back(quat2.value[0]);
            writeCursor.m_data_short.emplace_back(quat2.value[1]);
            break;
        }

        case QuatType::FULL_QUAT_NO_SIZE:
        {
            assert(quatTrack.m_frames.size() == 1);
            writeCursor.m_data_short.reserve(writeCursor.m_data_short.size() + 4);

            const auto& quat = quatTrack.m_frames[0];
            writeCursor.m_data_short.emplace_back(quat.value[0]);
            writeCursor.m_data_short.emplace_back(quat.value[1]);
            writeCursor.m_data_short.emplace_back(quat.value[2]);
            writeCursor.m_data_short.emplace_back(quat.value[3]);
            break;
        }
        }
    }

    void ProcessTransTrack(FlatData& writeCursor, const TransTrack& transTrack, const size_t boneIndex, const bool useByteIndices)
    {
        assert(boneIndex <= std::numeric_limits<uint8_t>::max());
        writeCursor.m_data_byte.emplace_back(static_cast<uint8_t>(boneIndex));

        switch (transTrack.m_type)
        {
        case TransType::SMALL_TRANS:
            WritePackedIndices(writeCursor, transTrack.m_indices, useByteIndices);
            WriteFloat3(writeCursor, transTrack.m_mins);
            WriteFloat3(writeCursor, transTrack.m_size);
            assert(transTrack.m_frames_u8.size() == transTrack.m_indices.size());

            writeCursor.m_random_data_byte.reserve(writeCursor.m_random_data_byte.size() + transTrack.m_frames_u8.size() * 3);
            for (const auto& vec : transTrack.m_frames_u8)
            {
                writeCursor.m_random_data_byte.emplace_back(vec.value[0]);
                writeCursor.m_random_data_byte.emplace_back(vec.value[1]);
                writeCursor.m_random_data_byte.emplace_back(vec.value[2]);
            }
            break;

        case TransType::FULL_TRANS:
            WritePackedIndices(writeCursor, transTrack.m_indices, useByteIndices);
            WriteFloat3(writeCursor, transTrack.m_mins);
            WriteFloat3(writeCursor, transTrack.m_size);
            assert(transTrack.m_frames_u16.size() == transTrack.m_indices.size());

            writeCursor.m_random_data_short.reserve(writeCursor.m_random_data_short.size() + transTrack.m_frames_u16.size() * 3);
            for (const auto& vec : transTrack.m_frames_u16)
            {
                writeCursor.m_random_data_short.emplace_back(vec.value[0]);
                writeCursor.m_random_data_short.emplace_back(vec.value[1]);
                writeCursor.m_random_data_short.emplace_back(vec.value[2]);
            }
            break;

        case TransType::TRANS_NO_SIZE:
            WriteFloat3(writeCursor, transTrack.m_constant);
            break;

        case TransType::NO_TRANS:
            break;
        }
    }
} // namespace

namespace xanim
{
    FlatData CreateFlatDataFromCommonXAnim(const CommonXAnimParts& parts)
    {
        FlatData writeCursor;

        const auto useByteIndices = parts.m_num_frames < 256;

        for (const auto& boneTrack : parts.m_bone_tracks)
            ProcessQuatTrack(writeCursor, boneTrack.m_quat, useByteIndices);

        const auto transBoneOrder = parts.GetBoneTrackOrderForTrans();
        const auto boneCount = transBoneOrder.size();
        for (size_t i = 0; i < boneCount; ++i)
        {
            const auto boneIndex = transBoneOrder[i];
            ProcessTransTrack(writeCursor, parts.m_bone_tracks[boneIndex].m_trans, boneIndex, useByteIndices);
        }

        return writeCursor;
    }
} // namespace xanim
