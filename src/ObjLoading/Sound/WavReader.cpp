#include "WavReader.h"

#include <Utils/Logging/Log.h>

WavReader::WavReader(std::istream& stream)
    : m_stream(stream)
{
}

bool WavReader::ReadExact(std::istream& s, void* buf, std::streamsize n)
{
    s.read(reinterpret_cast<char*>(buf), n);
    return s.gcount() == n;
}

uint32_t WavReader::MakeId(const char id[4])
{
    return static_cast<uint32_t>(static_cast<unsigned char>(id[0])) | (static_cast<uint32_t>(static_cast<unsigned char>(id[1])) << 8)
           | (static_cast<uint32_t>(static_cast<unsigned char>(id[2])) << 16) | (static_cast<uint32_t>(static_cast<unsigned char>(id[3])) << 24);
}

bool WavReader::ReadPcmHeader(WavMetaData& metaData, size_t& dataLen) const
{
    // Read RIFF header
    uint32_t chunkId = 0;
    uint32_t chunkSize = 0;
    if (!ReadExact(m_stream, &chunkId, sizeof(chunkId)))
        return false;
    if (!ReadExact(m_stream, &chunkSize, sizeof(chunkSize)))
        return false;

    if (chunkId != WAV_CHUNK_ID_RIFF)
        return false;

    // read WAVE id
    uint32_t waveId = 0;
    if (!ReadExact(m_stream, &waveId, sizeof(waveId)))
        return false;
    if (waveId != WAV_WAVE_ID)
        return false;

    bool foundFmt = false;
    bool foundData = false;
    WavFormatChunkPcm fmt{};
    uint32_t fmtChunkSize = 0;
    uint32_t dataChunkSize = 0;
    std::streampos chunkDataStart;

    while (!foundFmt || !foundData)
    {
        WavChunkHeader hdr;
        if (!ReadExact(m_stream, &hdr, sizeof(hdr)))
            return false;

        chunkDataStart = m_stream.tellg();
        if (hdr.chunkID == WAV_CHUNK_ID_FMT)
        {
            fmtChunkSize = hdr.chunkSize;
            if (fmtChunkSize < sizeof(WavFormatChunkPcm))
            {
                con::error(std::format("Unsupported fmt chunk size {}", fmtChunkSize));
                return false;
            }
            // PCM format header
            if (!ReadExact(m_stream, &fmt, static_cast<std::streamsize>(sizeof(WavFormatChunkPcm))))
                return false;

            uint32_t extra = fmtChunkSize - static_cast<uint32_t>(sizeof(WavFormatChunkPcm));
            if (extra > 0)
            {
                m_stream.seekg(static_cast<std::streamoff>(extra), std::ios::cur);
            }
            foundFmt = true;
        }
        else if (hdr.chunkID == WAV_CHUNK_ID_DATA)
        {
            dataChunkSize = hdr.chunkSize;
            foundData = true;
            break;
        }
        else
        {
            uint32_t skip = hdr.chunkSize;
            m_stream.seekg(static_cast<std::streamoff>(skip), std::ios::cur);
        }

        // Handle padding
        if ((hdr.chunkSize & 1) != 0)
        {
            m_stream.seekg(1, std::ios::cur);
        }

        if (!m_stream.good())
            return false;
    }

    if (!foundFmt || !foundData)
        return false;

    // Validate PCM format
    if (fmt.wFormatTag != WavFormat::PCM)
        return false;

    // Fill metaData
    metaData.channelCount = fmt.nChannels;
    metaData.samplesPerSec = fmt.nSamplesPerSec;
    metaData.bitsPerSample = fmt.wBitsPerSample;

    dataLen = static_cast<size_t>(dataChunkSize);
    return true;
}

bool WavReader::ReadPcmData(void* outBuffer, size_t dataLen) const
{
    if (!ReadExact(m_stream, outBuffer, static_cast<std::streamsize>(dataLen)))
        return false;
    return true;
}
