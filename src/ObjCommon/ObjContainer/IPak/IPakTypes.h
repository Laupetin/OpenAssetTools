#pragma once

#include <cstdint>

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
		IPakHash nameHash;
		IPakHash dataHash;
	};
	uint64_t combinedKey;
};

struct IPakIndexEntry
{
	IPakIndexEntryKey key;
	uint32_t offset;
	uint32_t size;
};

struct IPakDataChunkHeader
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