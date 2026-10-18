#include "Sound/WavWriter.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <string>

namespace
{
    uint32_t ReadU32(const std::string& data, const size_t offset)
    {
        uint32_t value;
        std::memcpy(&value, data.data() + offset, sizeof(value));
        return value;
    }

    TEST_CASE("WavWriter: RIFF size covers PCM payload and padding", "[wav][assetdumper]")
    {
        size_t dataSize = 22050; // SDK null.wav payload.
        unsigned bits = 16;
        SECTION("Even PCM payload") {}
        SECTION("Odd PCM payload")
        {
            dataSize = 3;
            bits = 8;
        }
        const std::string samples(dataSize, '\x12');
        std::ostringstream stream;
        WavWriter writer(stream);
        writer.WritePcmHeader({1, 22050, bits}, samples.size());
        writer.WritePcmData(samples.data(), samples.size());
        const auto wav = stream.str();
        REQUIRE(wav.size() == 44 + dataSize + (dataSize & 1u));
        REQUIRE(ReadU32(wav, 4) == wav.size() - 8);
        REQUIRE(ReadU32(wav, 40) == dataSize);
        REQUIRE(wav.substr(44, dataSize) == samples);
        if (dataSize & 1u)
            REQUIRE(wav.back() == '\0');
    }
} // namespace
