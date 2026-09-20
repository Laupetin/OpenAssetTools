#include "Zone/XBlock.h"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>

namespace
{
    TEST_CASE("Zone block buffers provide the maximum asset alignment", "[zone-loading][stream]")
    {
        XBlock block("test", 0, XBlockType::BLOCK_TYPE_NORMAL);
        constexpr std::array<size_t, 4> sizes{1u, 7u, 16u, 65537u};

        for (const auto size : sizes)
        {
            block.Alloc(size);
            REQUIRE(reinterpret_cast<std::uintptr_t>(block.m_buffer.get()) % 16u == 0u);
            REQUIRE(block.m_buffer_size == size);
        }

        block.Alloc(0u);
        REQUIRE(block.m_buffer.get() == nullptr);
        REQUIRE(block.m_buffer_size == 0u);
    }
} // namespace
