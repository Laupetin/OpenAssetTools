#pragma once

#include <cstdint>

#include "Utils/FileUtils.h"

namespace ipak_consts
{
    static constexpr uint32_t IPAK_MAGIC = FileUtils::MakeMagic32('K', 'A', 'P', 'I');
    static constexpr uint32_t IPAK_VERSION = 0x50000;

    static constexpr uint32_t IPAK_INDEX_SECTION = 1;
    static constexpr uint32_t IPAK_DATA_SECTION = 2;
    static constexpr uint32_t IPAK_BRANDING_SECTION = FileUtils::MakeMagic32('M', 'E', 'T', 'A');

    static constexpr size_t IPAK_CHUNK_SIZE = 0x8000;
    static constexpr size_t IPAK_CHUNK_COUNT_PER_READ = 0x8;
}

typedef uint32_t IPakHash;

struct IPakHeader
{
    uint32_t magic;
    uint32_t version;
    uint32_t size;
    uint32_t sectionCount;
};

struct IPakSection
{
    uint32_t type;
    uint32_t offset;
    uint32_t size;
    uint32_t itemCount;
};

union IPakIndexEntryKey
{
    struct
    {
        IPakHash dataHash;
        IPakHash nameHash;
    };

    uint64_t combinedKey;
};

struct IPakIndexEntry
{
    IPakIndexEntryKey key;
    uint32_t offset;
    uint32_t size;
};

struct IPakDataBlockHeader
{
    union
    {
        uint32_t countAndOffset;

        struct
        {
            uint32_t offset : 24;
            uint32_t count : 8;
        };
    };

    union
    {
        uint32_t commands[31];

        struct
        {
            uint32_t size : 24;
            uint32_t compressed : 8;
        } _commands[31];
    };
};
