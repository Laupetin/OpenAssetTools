#include "Sound/FlacDecoder.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace flac
{
    TEST_CASE("FlacDecoder: Ensure properly decodes flac stream info", "[sound][flac]")
    {
        // clang-format off
        constexpr uint8_t testData[]
        {
            // Magic
            'f', 'L', 'a', 'C',

            // Block header
            0x00, 0x00, 0x00, 0x22,

            // StreamInfo block
            0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0xB8, 0x02, 0xF0, 0x00,
            0x02, 0xF9, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
        };
        // clang-format on

        FlacMetaData metaData;
        const auto result = GetFlacMetaData(testData, sizeof(testData), metaData);

        REQUIRE(result == true);
        REQUIRE(metaData.m_minimum_block_size == 1024);
        REQUIRE(metaData.m_maximum_block_size == 1024);
        REQUIRE(metaData.m_minimum_frame_size == 0);
        REQUIRE(metaData.m_maximum_frame_size == 0);
        REQUIRE(metaData.m_sample_rate == 48000);
        REQUIRE(metaData.m_number_of_channels == 2);
        REQUIRE(metaData.m_bits_per_sample == 16);
        REQUIRE(metaData.m_total_samples == 194870);
    }
} // namespace flac
