#pragma once
#include <cstdint>
#include <limits>

struct ZoneHeader
{
    uint8_t m_magic[8];
    uint32_t m_version;
};

typedef uint16_t scr_string_t;
typedef uint32_t xchunk_size_t;
typedef uint32_t xblock_size_t;

constexpr uint16_t SCR_STRING_MAX = std::numeric_limits<scr_string_t>::max();

typedef unsigned block_t;
typedef unsigned asset_type_t;
typedef unsigned int zone_priority_t;
