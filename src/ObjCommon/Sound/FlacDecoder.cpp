#include "FlacDecoder.h"

#include "Utils/Alignment.h"
#include "Utils/ClassUtils.h"
#include "Utils/Endianness.h"
#include "Utils/FileUtils.h"

#include <cassert>
#include <iostream>
#include <sstream>

namespace
{
    constexpr auto FLAC_MAGIC = FileUtils::MakeMagic32('f', 'L', 'a', 'C');

    enum class MetaDataBlockType : unsigned
    {
        STREAMINFO = 0,
        PADDING = 1,
        APPLICATION = 2,
        SEEKTABLE = 3,
        VORBIS_COMMENT = 4,
        CUESHEET = 5,
        PICTURE = 6
    };

    struct MetaDataBlockHeader
    {
        uint8_t isLastMetaDataBlock;
        MetaDataBlockType blockType;
        uint32_t blockLength;
    };

    constexpr auto STREAM_INFO_BLOCK_SIZE = 34;

    class FlacReadingException final : public std::exception
    {
    public:
        explicit FlacReadingException(std::string message)
            : m_message(std::move(message))
        {
        }

        _NODISCARD char const* what() const noexcept override
        {
            return m_message.c_str();
        }

    private:
        std::string m_message;
    };

    class FlacBitReader
    {
    public:
        explicit FlacBitReader(std::istream& stream)
            : m_stream(stream),
              m_last_byte(0u),
              m_remaining_bits_last_byte(0u)
        {
        }

        template<typename T> T ReadBits(const size_t bitCount)
        {
            union
            {
                uint8_t buffer[sizeof(T)];
                T result;
            } data{};

            const auto byteCount = utils::Align(bitCount, 8u) / 8u;
            assert(byteCount <= sizeof(T));

            const auto shiftCount = (8u - bitCount % 8) % 8;

            auto remainingBits = bitCount;

#if HOST_ENDIANNESS == LITTLE_ENDIAN_ENDIANNESS
            auto offset = byteCount - 1;
#else
            auto offset = 0u;
#endif

            while (remainingBits > 0)
            {
                const auto curBits = static_cast<uint8_t>(std::min(remainingBits, 8u));

                if (m_remaining_bits_last_byte > 0)
                {
                    if (m_remaining_bits_last_byte < curBits)
                    {
                        const auto bitsFromFirstByte = m_remaining_bits_last_byte;
                        data.buffer[offset] = static_cast<uint8_t>(m_last_byte << (8u - bitsFromFirstByte));

                        m_stream.read(reinterpret_cast<char*>(&m_last_byte), sizeof(m_last_byte));
                        if (m_stream.gcount() != sizeof(m_last_byte))
                            throw FlacReadingException("Unexpected eof");

                        const auto bitsFromSecondByte = static_cast<uint8_t>(curBits - m_remaining_bits_last_byte);
                        m_remaining_bits_last_byte = 8u - bitsFromSecondByte;
                        const auto maskForSecondByte = static_cast<uint8_t>(0xFF << (8u - bitsFromSecondByte));
                        data.buffer[offset] |= (m_last_byte & maskForSecondByte) >> bitsFromFirstByte;
                    }
                    else if (m_remaining_bits_last_byte == curBits)
                    {
                        data.buffer[offset] = static_cast<uint8_t>(m_last_byte << (8u - curBits));
                        m_remaining_bits_last_byte = 0u;
                    }
                    else // m_remaining_bits_last_byte > curBits
                    {
                        const auto maskForCurBits = 0xFF >> (8u - curBits);
                        const auto maskForCurBitsInRemainingBits = static_cast<uint8_t>(maskForCurBits << (m_remaining_bits_last_byte - curBits));
                        const auto selectedData = static_cast<uint8_t>(m_last_byte & maskForCurBitsInRemainingBits);
                        data.buffer[offset] = static_cast<uint8_t>(selectedData << (8u - m_remaining_bits_last_byte));
                        m_remaining_bits_last_byte -= curBits;
                    }
                }
                else if (curBits >= 8u)
                {
                    m_stream.read(reinterpret_cast<char*>(&data.buffer[offset]), sizeof(uint8_t));
                    if (m_stream.gcount() != sizeof(uint8_t))
                        throw FlacReadingException("Unexpected eof");
                }
                else
                {
                    m_stream.read(reinterpret_cast<char*>(&m_last_byte), sizeof(m_last_byte));
                    if (m_stream.gcount() != sizeof(m_last_byte))
                        throw FlacReadingException("Unexpected eof");

                    data.buffer[offset] = m_last_byte & (0xFF << (8u - curBits));
                    m_remaining_bits_last_byte = static_cast<uint8_t>(8u - curBits);
                }

                remainingBits -= curBits;
#if HOST_ENDIANNESS == LITTLE_ENDIAN_ENDIANNESS
                --offset;
#else
                ++offset;
#endif
            }

            data.result >>= shiftCount;
            return data.result;
        }

        void ReadBuffer(void* buffer, const size_t bitCount)
        {
            assert(m_remaining_bits_last_byte == 0);
            assert(bitCount % 8 == 0);

            m_remaining_bits_last_byte = 0;
            m_stream.read(static_cast<char*>(buffer), bitCount / 8);
        }

        void Seek(const size_t offset)
        {
            assert(m_remaining_bits_last_byte == 0);
            m_remaining_bits_last_byte = 0;
            m_stream.seekg(offset, std::ios::cur);
        }

    private:
        std::istream& m_stream;
        uint8_t m_last_byte;
        uint8_t m_remaining_bits_last_byte;
    };
} // namespace

namespace flac
{
    FlacMetaData::FlacMetaData()
        : m_minimum_block_size(),
          m_maximum_block_size(),
          m_minimum_frame_size(),
          m_maximum_frame_size(),
          m_sample_rate(),
          m_number_of_channels(),
          m_bits_per_sample(),
          m_total_samples(),
          m_md5_signature{}
    {
    }

    void FlacReadStreamInfo(FlacBitReader& reader, FlacMetaData& metaData)
    {
        metaData.m_minimum_block_size = reader.ReadBits<uint16_t>(16);
        metaData.m_maximum_block_size = reader.ReadBits<uint16_t>(16);
        metaData.m_minimum_frame_size = reader.ReadBits<uint32_t>(24);
        metaData.m_maximum_frame_size = reader.ReadBits<uint32_t>(24);
        metaData.m_sample_rate = reader.ReadBits<uint32_t>(20);
        metaData.m_number_of_channels = static_cast<uint8_t>(reader.ReadBits<uint8_t>(3) + 1);
        metaData.m_bits_per_sample = static_cast<uint8_t>(reader.ReadBits<uint8_t>(5) + 1);
        metaData.m_total_samples = reader.ReadBits<uint64_t>(36);
        reader.ReadBuffer(metaData.m_md5_signature, 128);
    }

    bool GetFlacMetaData(std::istream& stream, FlacMetaData& metaData)
    {
        try
        {
            uint32_t readMagic;
            stream.read(reinterpret_cast<char*>(&readMagic), sizeof(readMagic));
            if (stream.gcount() != sizeof(readMagic) || readMagic != FLAC_MAGIC)
                throw FlacReadingException("Invalid flac magic");

            FlacBitReader reader(stream);
            while (true)
            {
                const MetaDataBlockHeader header{

                    reader.ReadBits<uint8_t>(1),
                    static_cast<MetaDataBlockType>(reader.ReadBits<uint8_t>(7)),
                    reader.ReadBits<uint32_t>(24),
                };

                if (header.blockType == MetaDataBlockType::STREAMINFO)
                {
                    if (header.blockLength != STREAM_INFO_BLOCK_SIZE)
                        throw FlacReadingException("Flac stream info block size invalid");

                    FlacReadStreamInfo(reader, metaData);
                    return true;
                }

                reader.Seek(header.blockLength * 8u);

                if (header.isLastMetaDataBlock)
                    break;
            }

            throw FlacReadingException("Missing flac stream info block");
        }
        catch (const FlacReadingException& e)
        {
            std::cerr << e.what() << "\n";
        }

        return false;
    }

    bool GetFlacMetaData(const void* data, const size_t dataSize, FlacMetaData& metaData)
    {
        std::istringstream ss(std::string(static_cast<const char*>(data), dataSize));
        return GetFlacMetaData(ss, metaData);
    }
} // namespace flac
