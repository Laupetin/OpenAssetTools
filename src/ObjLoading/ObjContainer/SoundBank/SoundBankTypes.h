#pragma once

#include "Utils/FileUtils.h"

#include <cstdint>

namespace sndbank_consts
{
    static constexpr uint32_t MAGIC = FileUtils::MakeMagic32('2', 'U', 'X', '#');
    static constexpr uint32_t VERSION = 14u;

    static constexpr unsigned OFFSET_DATA_START = 0x800;
}; // namespace sndbank_consts

struct SoundAssetBankChecksum
{
    uint8_t checksumBytes[16];
};

struct SoundAssetBankHeader
{
    uint32_t magic;                          // + 0x0
    uint32_t version;                        // + 0x4
    uint32_t entrySize;                      // + 0x8
    uint32_t checksumSize;                   // + 0xC
    uint32_t dependencySize;                 // + 0x10
    uint32_t entryCount;                     // + 0x14
    uint32_t dependencyCount;                // + 0x18
    uint32_t pad32;                          // + 0x1C
    int64_t fileSize;                        // + 0x20
    int64_t entryOffset;                     // + 0x28
    int64_t checksumOffset;                  // + 0x30
    SoundAssetBankChecksum checksumChecksum; // + 0x38
    char dependencies[512];                  // + 0x48
};

struct SoundAssetBankEntry
{
    uint32_t id;
    uint32_t size;
    uint32_t offset;
    uint32_t frameCount;
    uint8_t frameRateIndex;
    uint8_t channelCount;
    uint8_t looping;
    uint8_t format;
};
