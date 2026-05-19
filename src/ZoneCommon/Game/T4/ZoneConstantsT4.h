#pragma once
#include "Game/T4/T4.h"
#include "Zone/ZoneTypes.h"

#include <cstdint>
#include <string>

namespace T4
{
    class ZoneConstants final
    {
        ZoneConstants() = default;

    public:
        static constexpr const char* MAGIC_UNSIGNED = "IWffu100";
        static constexpr int ZONE_VERSION_PC = 387;

        static_assert(std::char_traits<char>::length(MAGIC_UNSIGNED) == sizeof(ZoneHeader::m_magic));

        static constexpr size_t AUTHED_CHUNK_SIZE = 0x2000;
        static constexpr unsigned OFFSET_BLOCK_BIT_COUNT = 3u;
        static constexpr block_t INSERT_BLOCK = XFILE_BLOCK_VIRTUAL;
    };
} // namespace T4
