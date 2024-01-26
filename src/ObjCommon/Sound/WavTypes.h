#pragma once
#include "Utils/FileUtils.h"

#include <cstdint>

constexpr uint32_t WAV_WAVE_ID = FileUtils::MakeMagic32('W', 'A', 'V', 'E');
constexpr uint32_t WAV_CHUNK_ID_RIFF = FileUtils::MakeMagic32('R', 'I', 'F', 'F');
constexpr uint32_t WAV_CHUNK_ID_FMT = FileUtils::MakeMagic32('f', 'm', 't', ' ');
constexpr uint32_t WAV_CHUNK_ID_DATA = FileUtils::MakeMagic32('d', 'a', 't', 'a');

struct WavChunkHeader
{
    uint32_t chunkID;
    uint32_t chunkSize;
};

enum class WavFormat : int16_t
{
    PCM = 1
};

struct WavFormatChunkPcm
{
    WavFormat wFormatTag;
    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;
};

struct WavMetaData
{
    unsigned channelCount;
    unsigned samplesPerSec;
    unsigned bitsPerSample;
};

struct WavHeader
{
    unsigned int chunkIdRiff;
    unsigned int chunkIdSize;
    unsigned int format;
    WavChunkHeader chunkHeader;
    WavFormatChunkPcm formatChunk;
    WavChunkHeader subChunkHeader;
};