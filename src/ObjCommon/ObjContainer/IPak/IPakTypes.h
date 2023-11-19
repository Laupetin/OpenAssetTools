#pragma once

#include "Utils/FileUtils.h"

#include <cstdint>

namespace ipak_consts
{
    static constexpr uint32_t IPAK_MAGIC = FileUtils::MakeMagic32('K', 'A', 'P', 'I');
    static constexpr uint32_t IPAK_VERSION = 0x50000;

    static constexpr uint32_t IPAK_INDEX_SECTION = 1;
    static constexpr uint32_t IPAK_DATA_SECTION = 2;
    static constexpr uint32_t IPAK_BRANDING_SECTION = FileUtils::MakeMagic32('M', 'E', 'T', 'A');

    static constexpr size_t IPAK_CHUNK_SIZE = 0x8000;
    static constexpr size_t IPAK_CHUNK_COUNT_PER_READ = 0x8;

    static constexpr uint32_t IPAK_COMMAND_DEFAULT_SIZE = 0x7F00;
    static constexpr uint32_t IPAK_COMMAND_UNCOMPRESSED = 0;
    static constexpr uint32_t IPAK_COMMAND_COMPRESSED = 1;
    static constexpr uint32_t IPAK_COMMAND_SKIP = 0xCF;

    static_assert(IPAK_COMMAND_DEFAULT_SIZE <= IPAK_CHUNK_SIZE);
} // namespace ipak_consts

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

struct IPakDataBlockCountAndOffset
{
    uint32_t offset : 24;
    uint32_t count : 8;
};

static_assert(sizeof(IPakDataBlockCountAndOffset) == 4);

struct IPakDataBlockCommand
{
    uint32_t size : 24;
    uint32_t compressed : 8;
};

static_assert(sizeof(IPakDataBlockCommand) == 4);

struct IPakDataBlockHeader
{
    IPakDataBlockCountAndOffset countAndOffset;
    IPakDataBlockCommand commands[31];
};

static_assert(sizeof(IPakDataBlockHeader) == 128);
