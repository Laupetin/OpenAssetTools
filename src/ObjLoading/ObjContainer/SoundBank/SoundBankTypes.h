#pragma once

#include <cstdint>

class SoundBankConsts
{
    SoundBankConsts() = default;

public:
    static constexpr unsigned OFFSET_DATA_START = 0x800;
};

struct SoundAssetBankChecksum
{
    char checksumBytes[16];
};

struct SoundAssetBankHeader
{
    unsigned int magic;                     // + 0x0
    unsigned int version;                   // + 0x4
    unsigned int entrySize;                 // + 0x8
    unsigned int checksumSize;              // + 0xC
    unsigned int dependencySize;            // + 0x10
    unsigned int entryCount;                // + 0x14
    unsigned int dependencyCount;           // + 0x18
    unsigned int pad32;                     // + 0x1C
    int64_t fileSize;                       // + 0x20
    int64_t entryOffset;                    // + 0x28
    int64_t checksumOffset;                 // + 0x30
    SoundAssetBankChecksum checksumChecksum;  // + 0x38
    char dependencies[512];                 // + 0x48
};

struct SoundAssetBankEntry
{
    unsigned int id;
    unsigned int size;
    unsigned int offset;
    unsigned int frameCount;
    unsigned char frameRateIndex;
    unsigned char channelCount;
    unsigned char looping;
    unsigned char format;
};
