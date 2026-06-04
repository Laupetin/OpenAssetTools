#include "FlatXAnimReader.h"

#include <cassert>
#include <cstring>
#include <format>
#include <utility>

using namespace xanim;

namespace
{

    [[nodiscard]] std::vector<uint16_t> ReadPackedIndices(FlatXAnimReadCursor& cursor, const uint16_t storedSize, const bool useByteIndices)
    {
        const auto count = static_cast<size_t>(storedSize) + 1uz;
        std::vector<uint16_t> result(count);

        if (useByteIndices)
        {
            for (auto i = 0uz; i < count; i++)
                result[i] = cursor.PopDataByte();

            return result;
        }

        // The linker moves 16-bit frame indices into the top-level indices pool only when
        // the in-memory stored size is at least 64, i.e. frameCount >= 65.
        if (storedSize >= 64u)
        {
            cursor.ReadIndices(result.data(), count);

            // The game inserts checkpoint values in dataShort
            // Those checkpoint values are copied from positions in the full index list: the first entry, then every 256th entry, and always the final entry.
            // The final entry is included even when it does not land exactly on a 256-entry boundary.
            cursor.SkipDataShort(((count - 2uz) / 256u) + 2uz);
            return result;
        }

        cursor.ReadDataShort(result.data(), count);
        return result;
    }

    [[nodiscard]] float IntBitsToFloat(const int value)
    {
        union
        {
            int i;
            float f;
        };

        i = value;
        return f;
    }

    [[nodiscard]] std::array<float, 3> ReadFloat3(FlatXAnimReadCursor& cursor)
    {
        std::array<float, 3> result{};
        for (float& i : result)
            i = IntBitsToFloat(cursor.PopDataInt());
        return result;
    }
} // namespace

namespace xanim
{
    XAnimBoneCounts::XAnimBoneCounts(const size_t noQuatCount,
                                     const size_t halfQuatCount,
                                     const size_t fullQuatCount,
                                     const size_t halfQuatNoSizeCount,
                                     const size_t fullQuatNoSizeCount,
                                     const size_t smallTransCount,
                                     const size_t fullTransCount,
                                     const size_t transNoSizeCount,
                                     const size_t noTransCount)
        : m_counts({
              noQuatCount,
              halfQuatCount,
              fullQuatCount,
              halfQuatNoSizeCount,
              fullQuatNoSizeCount,
              smallTransCount,
              fullTransCount,
              transNoSizeCount,
              noTransCount,
          })
    {
        assert(m_counts[std::to_underlying(QuatType::NO_QUAT)] == noQuatCount);
        assert(m_counts[std::to_underlying(QuatType::HALF_QUAT)] == halfQuatCount);
        assert(m_counts[std::to_underlying(QuatType::FULL_QUAT)] == fullQuatCount);
        assert(m_counts[std::to_underlying(QuatType::HALF_QUAT_NO_SIZE)] == halfQuatNoSizeCount);
        assert(m_counts[std::to_underlying(QuatType::FULL_QUAT_NO_SIZE)] == fullQuatNoSizeCount);

        assert(m_counts[std::to_underlying(TransType::SMALL_TRANS)] == smallTransCount);
        assert(m_counts[std::to_underlying(TransType::FULL_TRANS)] == fullTransCount);
        assert(m_counts[std::to_underlying(TransType::TRANS_NO_SIZE)] == transNoSizeCount);
        assert(m_counts[std::to_underlying(TransType::NO_TRANS)] == noTransCount);

        assert(noQuatCount + halfQuatCount + fullQuatCount + halfQuatNoSizeCount + fullQuatNoSizeCount
               == smallTransCount + fullTransCount + transNoSizeCount + noTransCount);
    }

    size_t XAnimBoneCounts::GetCountForQuatType(const QuatType quatType) const
    {
        return m_counts[std::to_underlying(quatType)];
    }

    size_t XAnimBoneCounts::GetCountForTransType(const TransType transType) const
    {
        return m_counts[std::to_underlying(transType)];
    }

    FlatDataReadException::FlatDataReadException(std::string message)
        : m_message(std::move(message))
    {
    }

    const char* FlatDataReadException::what() const noexcept
    {
        return m_message.c_str();
    }

    const std::string& FlatDataReadException::message() const
    {
        return m_message;
    }

    FlatXAnimReadCursor::FlatXAnimReadCursor(uint8_t* dataByte,
                                             const size_t dataByteCount,
                                             int16_t* dataShort,
                                             const size_t dataShortCount,
                                             int32_t* dataInt,
                                             const size_t dataIntCount,
                                             uint8_t* randomDataByte,
                                             const size_t randomDataByteCount,
                                             int16_t* randomDataShort,
                                             const size_t randomDataShortCount,
                                             uint16_t* indices,
                                             const size_t indicesCount)
        : m_data_byte(dataByte),
          m_data_byte_count(dataByteCount),
          m_data_short(dataShort),
          m_data_short_count(dataShortCount),
          m_data_int(dataInt),
          m_data_int_count(dataIntCount),
          m_random_data_byte(randomDataByte),
          m_random_data_byte_count(randomDataByteCount),
          m_random_data_short(randomDataShort),
          m_random_data_short_count(randomDataShortCount),
          m_indices(indices),
          m_indices_count(indicesCount)
    {
    }

#define DATA_EXHAUSTED_ERROR(name, readCount, remainingCount)                                                                                                  \
    FlatDataReadException(std::format("Exhausted {} while trying to read {} entries ({} remaining)", name, readCount, remainingCount))

    uint8_t FlatXAnimReadCursor::PopDataByte()
    {
        if (m_data_byte_count < 1)
            throw DATA_EXHAUSTED_ERROR("dataByte", 1, m_data_byte_count);

        const auto result = m_data_byte[0];
        m_data_byte++;
        m_data_byte_count--;

        return result;
    }

    int16_t FlatXAnimReadCursor::PopDataShort()
    {
        if (m_data_short_count < 1)
            throw DATA_EXHAUSTED_ERROR("dataShort", 1, m_data_short_count);

        const auto result = m_data_short[0];
        m_data_short++;
        m_data_short_count--;

        return result;
    }

    void FlatXAnimReadCursor::ReadDataShort(void* dst, const size_t count)
    {
        if (m_data_short_count < count)
            throw DATA_EXHAUSTED_ERROR("dataShort", count, m_data_short_count);

        std::memcpy(dst, m_data_short, count * sizeof(int16_t));

        m_data_short += count;
        m_data_short_count -= count;
    }

    void FlatXAnimReadCursor::SkipDataShort(const size_t count)
    {
        if (m_data_short_count < count)
            throw DATA_EXHAUSTED_ERROR("dataShort", count, m_data_short_count);

        m_data_short += count;
        m_data_short_count -= count;
    }

    int32_t FlatXAnimReadCursor::PopDataInt()
    {
        if (m_data_int_count < 1)
            throw DATA_EXHAUSTED_ERROR("dataInt", 1, m_data_int_count);

        const auto result = m_data_int[0];
        m_data_int++;
        m_data_int_count--;

        return result;
    }

    void FlatXAnimReadCursor::ReadRandomDataByte(void* dst, const size_t count)
    {
        if (m_random_data_byte_count < count)
            throw DATA_EXHAUSTED_ERROR("randomDataByte", count, m_random_data_byte_count);

        std::memcpy(dst, m_random_data_byte, count * sizeof(uint8_t));

        m_random_data_byte += count;
        m_random_data_byte_count -= count;
    }

    void FlatXAnimReadCursor::ReadRandomDataShort(void* dst, const size_t count)
    {
        if (m_random_data_short_count < count)
            throw DATA_EXHAUSTED_ERROR("randomDataShort", count, m_random_data_short_count);

        std::memcpy(dst, m_random_data_short, count * sizeof(int16_t));

        m_random_data_short += count;
        m_random_data_short_count -= count;
    }

    void FlatXAnimReadCursor::ReadIndices(void* dst, const size_t count)
    {
        if (m_indices_count < count)
            throw DATA_EXHAUSTED_ERROR("indices", count, m_indices_count);

        std::memcpy(dst, m_indices, count * sizeof(uint16_t));

        m_indices += count;
        m_indices_count -= count;
    }

    std::expected<void, std::string> FlatXAnimReadCursor::ExpectEndOfData() const
    {
#define END_OF_DATA_ERROR(name, size) std::unexpected(std::format("Expected {} to be exhausted but {} bytes remain", name, size))
#define CHECK_END_OF_DATA_ERROR(name, size)                                                                                                                    \
    if ((size) > 0)                                                                                                                                            \
        return END_OF_DATA_ERROR(name, size);

        CHECK_END_OF_DATA_ERROR("dataByte", m_data_byte_count)
        CHECK_END_OF_DATA_ERROR("dataShort", m_data_short_count)
        CHECK_END_OF_DATA_ERROR("dataInt", m_data_int_count)
        CHECK_END_OF_DATA_ERROR("randomDataByte", m_random_data_byte_count)
        CHECK_END_OF_DATA_ERROR("randomDataShort", m_random_data_short_count)
        CHECK_END_OF_DATA_ERROR("indices", m_indices_count)

        return {};

#undef END_OF_DATA_ERROR
#undef CHECK_END_OF_DATA_ERROR
    }

    std::expected<std::vector<BoneTrack>, std::string> CreateBoneTracksFromFlatData(std::vector<std::string> boneNames,
                                                                                    const XAnimBoneCounts& boneCounts,
                                                                                    FlatXAnimReadCursor& cursor,
                                                                                    const bool useByteIndices)
    {
        const auto boneCount = boneNames.size();
        std::vector<BoneTrack> boneTracks(boneCount);
        for (size_t i = 0; i < boneCount; i++)
            boneTracks[i].m_name = std::move(boneNames[i]);

        size_t boneIndex = 0;

        try
        {
            for (auto i = 0u; i < boneCounts.GetCountForQuatType(QuatType::NO_QUAT); i++, boneIndex++)
                boneTracks[boneIndex].m_quat.m_type = QuatType::NO_QUAT;

            for (auto i = 0u; i < boneCounts.GetCountForQuatType(QuatType::HALF_QUAT); i++, boneIndex++)
            {
                auto& quat = boneTracks[boneIndex].m_quat;
                quat.m_type = QuatType::HALF_QUAT;
                const auto storedSize = static_cast<uint16_t>(cursor.PopDataShort());
                const auto frameCount = static_cast<size_t>(storedSize) + 1uz;
                quat.m_indices = ReadPackedIndices(cursor, storedSize, useByteIndices);

                static_assert(sizeof(decltype(quat.m_frames2)::value_type) == sizeof(int16_t) * 2);
                quat.m_frames2.resize(frameCount);
                cursor.ReadRandomDataShort(quat.m_frames2.data(), frameCount * 2);
            }

            for (auto i = 0u; i < boneCounts.GetCountForQuatType(QuatType::FULL_QUAT); i++, boneIndex++)
            {
                auto& quat = boneTracks[boneIndex].m_quat;
                quat.m_type = QuatType::FULL_QUAT;
                const auto storedSize = static_cast<uint16_t>(cursor.PopDataShort());
                const auto frameCount = static_cast<size_t>(storedSize) + 1uz;
                quat.m_indices = ReadPackedIndices(cursor, storedSize, useByteIndices);

                static_assert(sizeof(decltype(quat.m_frames)::value_type) == sizeof(int16_t) * 4);
                quat.m_frames.resize(frameCount);
                cursor.ReadRandomDataShort(quat.m_frames.data(), frameCount * 4);
            }

            for (auto i = 0u; i < boneCounts.GetCountForQuatType(QuatType::HALF_QUAT_NO_SIZE); i++, boneIndex++)
            {
                auto& quat = boneTracks[boneIndex].m_quat;
                quat.m_type = QuatType::HALF_QUAT_NO_SIZE;

                static_assert(sizeof(decltype(quat.m_frames2)::value_type) == sizeof(int16_t) * 2);
                quat.m_frames2.resize(1);
                cursor.ReadDataShort(quat.m_frames2.data(), 2);
            }

            for (auto i = 0u; i < boneCounts.GetCountForQuatType(QuatType::FULL_QUAT_NO_SIZE); i++, boneIndex++)
            {
                auto& quat = boneTracks[boneIndex].m_quat;
                quat.m_type = QuatType::FULL_QUAT_NO_SIZE;

                static_assert(sizeof(decltype(quat.m_frames)::value_type) == sizeof(int16_t) * 4);
                quat.m_frames.resize(1);
                cursor.ReadDataShort(quat.m_frames.data(), 4);
            }

            std::vector<bool> transAssigned(boneCount, false);

            for (auto i = 0u; i < boneCounts.GetCountForTransType(TransType::SMALL_TRANS); i++)
            {
                const auto bone = static_cast<size_t>(cursor.PopDataByte());
                assert(bone < boneCount && !transAssigned[bone]);

                auto& trans = boneTracks[bone].m_trans;
                transAssigned[bone] = true;
                trans.m_type = TransType::SMALL_TRANS;

                const auto storedSize = static_cast<uint16_t>(cursor.PopDataShort());
                const auto frameCount = static_cast<size_t>(storedSize) + 1uz;
                trans.m_mins = ReadFloat3(cursor);
                trans.m_size = ReadFloat3(cursor);
                trans.m_indices = ReadPackedIndices(cursor, storedSize, useByteIndices);

                static_assert(sizeof(decltype(trans.m_byte_frames)::value_type) == sizeof(uint8_t));
                trans.m_byte_frames.resize(frameCount * 3uz);
                cursor.ReadRandomDataByte(trans.m_byte_frames.data(), frameCount * 3uz);
            }

            for (auto i = 0u; i < boneCounts.GetCountForTransType(TransType::FULL_TRANS); i++)
            {
                const auto bone = static_cast<size_t>(cursor.PopDataByte());
                assert(bone < boneCount && !transAssigned[bone]);

                auto& trans = boneTracks[bone].m_trans;
                transAssigned[bone] = true;
                trans.m_type = TransType::FULL_TRANS;

                const auto storedSize = static_cast<uint16_t>(cursor.PopDataShort());
                const auto frameCount = static_cast<size_t>(storedSize) + 1uz;
                trans.m_mins = ReadFloat3(cursor);
                trans.m_size = ReadFloat3(cursor);
                trans.m_indices = ReadPackedIndices(cursor, storedSize, useByteIndices);

                static_assert(sizeof(decltype(trans.m_short_frames)::value_type) == sizeof(int16_t));
                trans.m_short_frames.resize(frameCount * 3uz);
                cursor.ReadRandomDataShort(trans.m_short_frames.data(), frameCount * 3uz);
            }

            for (auto i = 0u; i < boneCounts.GetCountForTransType(TransType::TRANS_NO_SIZE); i++)
            {
                const auto bone = static_cast<size_t>(cursor.PopDataByte());
                assert(bone < boneCount && !transAssigned[bone]);

                auto& trans = boneTracks[bone].m_trans;
                transAssigned[bone] = true;
                trans.m_type = TransType::TRANS_NO_SIZE;
                trans.m_constant = ReadFloat3(cursor);
            }

            for (auto i = 0u; i < boneCounts.GetCountForTransType(TransType::NO_TRANS); i++)
            {
                const auto bone = static_cast<size_t>(cursor.PopDataByte());
                assert(bone < boneCount && !transAssigned[bone]);

                boneTracks[bone].m_trans.m_type = TransType::NO_TRANS;
                transAssigned[bone] = true;
            }

            for (auto i = 0uz; i < boneCount; i++)
                assert(transAssigned[i]);
        }
        catch (const FlatDataReadException& exception)
        {
            return std::unexpected(exception.message());
        }

        auto maybeError = cursor.ExpectEndOfData();
        if (!maybeError.has_value())
            return std::unexpected(std::move(maybeError).error());

        return boneTracks;
    }
} // namespace xanim
