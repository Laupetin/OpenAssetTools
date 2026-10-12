#include "Sound/WavWriter.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <cstring>
#include <sstream>
#include <string>

TEST_CASE("WavWriter: RIFF contains the complete PCM payload and alignment padding", "[sound][wav]")
{
    const auto size = GENERATE(0uz, 1uz, 2uz, 31uz, 2048uz);
    const std::string samples(size, static_cast<char>(0x81));
    std::ostringstream output(std::ios::binary);
    const WavWriter writer(output);
    writer.WritePcmHeader({1, 22050, 8}, samples.size());
    writer.WritePcmData(samples.data(), samples.size());
    const auto bytes = output.str();
    REQUIRE(bytes.size() >= sizeof(WavHeader));
    WavHeader header{};
    std::memcpy(&header, bytes.data(), sizeof(header));
    CHECK(header.chunkIdRiff == WAV_CHUNK_ID_RIFF);
    CHECK(header.chunkIdSize == bytes.size() - 8u);
    CHECK(header.format == WAV_WAVE_ID);
    CHECK(header.chunkHeader.chunkID == WAV_CHUNK_ID_FMT);
    CHECK(header.chunkHeader.chunkSize == sizeof(WavFormatChunkPcm));
    CHECK(header.formatChunk.wFormatTag == WavFormat::PCM);
    CHECK(header.formatChunk.nChannels == 1u);
    CHECK(header.formatChunk.nSamplesPerSec == 22050u);
    CHECK(header.formatChunk.nAvgBytesPerSec == 22050u);
    CHECK(header.formatChunk.nBlockAlign == 1u);
    CHECK(header.formatChunk.wBitsPerSample == 8u);
    CHECK(header.subChunkHeader.chunkID == WAV_CHUNK_ID_DATA);
    REQUIRE(header.subChunkHeader.chunkSize == samples.size());
    CHECK(bytes.substr(sizeof(header), samples.size()) == samples);
    CHECK(bytes.size() % 2u == 0u);
    if (samples.size() & 1u)
        CHECK(bytes.back() == '\0');
    else
        CHECK(bytes.size() == sizeof(header) + samples.size());
}
