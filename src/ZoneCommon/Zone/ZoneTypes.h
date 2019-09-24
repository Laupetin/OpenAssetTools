#pragma once
#include <cstdint>

struct ZoneHeader
{
    uint8_t m_magic[8];
    uint32_t m_version;
};

#ifdef _WIN64
typedef uint32_t scr_string_t;
typedef uint64_t xchunk_size_t;
typedef uint64_t xblock_size_t;
typedef uint64_t zone_pointer_t;
#elif _WIN32
typedef uint16_t scr_string_t;
typedef uint32_t xchunk_size_t;
typedef uint32_t xblock_size_t;
typedef uint32_t zone_pointer_t;
#endif

typedef int block_t;
typedef int asset_type_t;
typedef unsigned int zone_priority_t;