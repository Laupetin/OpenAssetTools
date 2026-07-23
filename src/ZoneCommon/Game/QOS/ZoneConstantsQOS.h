#pragma once
#include "Game/QOS/QOS.h"
#include "Zone/ZoneTypes.h"

#include <cstddef>
#include <cstdint>

namespace QOS
{
    class ZoneConstants final
    {
        ZoneConstants() = default;

    public:
        static constexpr int ZONE_VERSION_PC = 470;
        static constexpr size_t FILE_HEADER_SIZE = 28;
        static constexpr size_t AUTHED_CHUNK_SIZE = 0x20000;

        static constexpr unsigned OFFSET_BLOCK_BIT_COUNT = 3u;
        static constexpr block_t INSERT_BLOCK = XFILE_BLOCK_VIRTUAL;
    };
} // namespace QOS
