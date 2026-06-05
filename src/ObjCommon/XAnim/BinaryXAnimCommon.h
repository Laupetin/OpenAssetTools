#pragma once

#include <cstdint>

namespace xanim
{
    enum class CompiledXAnimVersion : uint8_t
    {
        // IW3, T4
        VERSION_17 = 17,
        // T5, T6
        VERSION_19 = 19
    };

    namespace binary17
    {
        constexpr uint8_t FLAG_LOOPED = 1u;
        constexpr uint8_t FLAG_DELTA = 2u;
    } // namespace binary17

    namespace binary19
    {
        constexpr uint8_t FLAG_LOOPED = 1u;
        constexpr uint8_t FLAG_DELTA = 2u;
        constexpr uint8_t FLAG_LEFT_HAND_GRIP_IK = 4u;
        constexpr uint8_t FLAG_STREAMABLE = 8u;
    } // namespace binary19
} // namespace xanim
