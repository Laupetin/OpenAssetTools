#pragma once

#include <cstdint>

namespace xanim
{
    enum class CompiledXAnimVersion : uint8_t
    {
        // IW3, T4
        VERSION_17 = 17,
        // IW4, IW5
        VERSION_18 = 18,
        // T5, T6 (slightly different format however)
        VERSION_19 = 19
    };

    namespace binary17
    {
        constexpr uint8_t FLAG_LOOPED = 0x1;
        constexpr uint8_t FLAG_DELTA = 0x2;
    } // namespace binary17

    namespace binary18
    {
        constexpr uint8_t FLAG_LOOPED = 0x1;
        constexpr uint8_t FLAG_DELTA = 0x2;
        constexpr uint8_t FLAG_DELTA_3D = 0x4;
    } // namespace binary18

    namespace binary19
    {
        constexpr uint8_t FLAG_LOOPED = 0x1;
        constexpr uint8_t FLAG_DELTA = 0x2;

        constexpr uint8_t FLAG_T5_LEFT_HAND_GRIP_IK = 0x4;
        constexpr uint8_t FLAG_T5_STREAMABLE = 0x8;

        constexpr uint8_t FLAG_T6_DELTA_3D = 0x4;
        constexpr uint8_t FLAG_T6_LEFT_HAND_GRIP_IK = 0x8;

        // This flag is not part of the official format.
        // T5 and T6 use the same XAnim version, even though the format is different:
        // * Flags have slightly different values
        // * T6 does not support "streamable"
        // * T5 does not support delta3D
        // So this flag value is added to be able to identify whether the file should use
        // T5 or T6 parsing behavior.
        constexpr uint8_t FLAG_T6_COMPATIBILITY = 0x80;
    } // namespace binary19
} // namespace xanim
