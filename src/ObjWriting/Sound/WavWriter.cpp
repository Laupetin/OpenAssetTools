#include "WavWriter.h"

WavWriter::WavWriter(std::ostream& stream)
    : m_stream(stream)
{
}

void WavWriter::WritePcmHeader(const WavMetaData& metaData, const size_t dataLen) const
{
    constexpr auto riffMasterChunkSize = static_cast<uint32_t>(sizeof(WAV_CHUNK_ID_RIFF) + sizeof(uint32_t) + sizeof(WAV_WAVE_ID) + sizeof(WavChunkHeader)
                                                               + sizeof(WavFormatChunkPcm) + sizeof(WavChunkHeader) + sizeof(uint32_t));

    m_stream.write(reinterpret_cast<const char*>(&WAV_CHUNK_ID_RIFF), sizeof(WAV_CHUNK_ID_RIFF));
    m_stream.write(reinterpret_cast<const char*>(&riffMasterChunkSize), sizeof(riffMasterChunkSize));
    m_stream.write(reinterpret_cast<const char*>(&WAV_WAVE_ID), sizeof(WAV_WAVE_ID));

    constexpr WavChunkHeader formatChunkHeader{.chunkID = WAV_CHUNK_ID_FMT, .chunkSize = sizeof(WavFormatChunkPcm)};
    m_stream.write(reinterpret_cast<const char*>(&formatChunkHeader), sizeof(formatChunkHeader));

    const WavFormatChunkPcm formatChunk{
        .wFormatTag = WavFormat::PCM,
        .nChannels = static_cast<uint16_t>(metaData.channelCount),
        .nSamplesPerSec = metaData.samplesPerSec,
        .nAvgBytesPerSec = metaData.samplesPerSec * metaData.channelCount * metaData.bitsPerSample / 8,
        .nBlockAlign = static_cast<uint16_t>(metaData.channelCount * (metaData.bitsPerSample / 8)),
        .wBitsPerSample = static_cast<uint16_t>(metaData.bitsPerSample),
    };
    m_stream.write(reinterpret_cast<const char*>(&formatChunk), sizeof(formatChunk));

    const WavChunkHeader dataChunkHeader{.chunkID = WAV_CHUNK_ID_DATA, .chunkSize = static_cast<uint32_t>(dataLen)};
    m_stream.write(reinterpret_cast<const char*>(&dataChunkHeader), sizeof(dataChunkHeader));
}

void WavWriter::WritePcmData(const void* data, const size_t dataLen) const
{
    m_stream.write(static_cast<const char*>(data), static_cast<std::streamsize>(dataLen));
}
