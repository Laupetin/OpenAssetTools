#pragma once

#include <cctype>
#include <cstdint>

// This header contains multiple varying implementations of the DJB2 algorithm:
// http://www.cse.yorku.ca/~oz/hash.html

constexpr uint32_t DJB2_STARTING_VALUE = 5381;

static constexpr uint32_t djb2(const char* str, uint32_t hash)
{
    for (char c = *str; c; c = *++str)
    {
        // hash * 33 + c
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

static constexpr uint32_t djb2(const char* str)
{
    return djb2(str, DJB2_STARTING_VALUE);
}

static constexpr uint32_t djb2_nocase(const char* str, uint32_t hash)
{
    for (char c = *str; c; c = *++str)
    {
        // Or with 0x20 makes the string case-insensitive
        // but also messes up non-letters
        hash = ((hash << 5) + hash) + (c | 0x20);
    }

    return hash;
}

static constexpr uint32_t djb2_nocase(const char* str)
{
    return djb2_nocase(str, DJB2_STARTING_VALUE);
}

static constexpr uint32_t djb2_lower(const char* str, uint32_t hash)
{
    for (char c = *str; c; c = *++str)
        hash = ((hash << 5) + hash) + std::tolower(c);

    return hash;
}

static constexpr uint32_t djb2_lower(const char* str)
{
    return djb2_lower(str, DJB2_STARTING_VALUE);
}

static constexpr uint32_t djb2_xor(const char* str, uint32_t hash)
{
    for (char c = *str; c; c = *++str)
    {
        hash = ((hash << 5) + hash) ^ c;
    }

    return hash;
}

static constexpr uint32_t djb2_xor(const char* str)
{
    return djb2_xor(str, DJB2_STARTING_VALUE);
}

static constexpr uint32_t djb2_xor_nocase(const char* str, uint32_t hash)
{
    for (char c = *str; c; c = *++str)
    {
        // Or with 0x20 makes the string case-insensitive
        // but also messes up non-letters
        hash = ((hash << 5) + hash) ^ (c | 0x20);
    }

    return hash;
}

static constexpr uint32_t djb2_xor_nocase(const char* str)
{
    return djb2_xor_nocase(str, DJB2_STARTING_VALUE);
}

static constexpr uint32_t djb2_xor_lower(const char* str, uint32_t hash)
{
    for (char c = *str; c; c = *++str)
        hash = ((hash << 5) + hash) ^ std::tolower(c);

    return hash;
}

static constexpr uint32_t djb2_xor_lower(const char* str)
{
    return djb2_xor_lower(str, DJB2_STARTING_VALUE);
}
