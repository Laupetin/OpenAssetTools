#pragma once

#include <cstdint>

typedef uint32_t IPakHash;

namespace ipak_consts
{
	static constexpr size_t IPAK_CHUNK_SIZE = 0x8000;
	static constexpr size_t IPAK_CHUNK_COUNT_PER_READ = 0x8;
}

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
		}_commands[31];
	};
};