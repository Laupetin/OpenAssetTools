#include "Zone/XBlock.h"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>

namespace
{
    TEST_CASE("Zone block buffers provide the maximum asset alignment", "[zone-loading][stream]")
    {
        static constexpr std::uintptr_t REQUIRED_ALIGNMENT = 4096u;

        XBlock block("test", 0, XBlockType::BLOCK_TYPE_NORMAL);
        constexpr size_t sizes[]{1u, 7u, 16u, 4096u, 65537u};

        for (const auto size : sizes)
        {
            block.Alloc(size);
            REQUIRE(reinterpret_cast<std::uintptr_t>(block.m_buffer.get()) % REQUIRED_ALIGNMENT == 0u);
            REQUIRE(block.m_buffer_size == size);
        }

        block.Alloc(0u);
        REQUIRE(block.m_buffer.get() == nullptr);
        REQUIRE(block.m_buffer_size == 0u);
    }
} // namespace
