#pragma once

#include <cstdint>
#include <istream>

namespace flac
{
    class FlacMetaData
    {
    public:
        uint16_t m_minimum_block_size;
        uint16_t m_maximum_block_size;
        uint32_t m_minimum_frame_size;
        uint32_t m_maximum_frame_size;
        uint32_t m_sample_rate;
        uint8_t m_number_of_channels;
        uint8_t m_bits_per_sample;
        uint64_t m_total_samples;
        uint8_t m_md5_signature[16];

        FlacMetaData();
    };

    bool GetFlacMetaData(std::istream& stream, FlacMetaData& metaData);
    bool GetFlacMetaData(const void* data, size_t dataSize, FlacMetaData& metaData);
} // namespace flac
