#include "Pack.h"

#include "HalfFloat.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

union PackUtil32
{
    uint32_t u;
    int32_t i;
    float f;
    int8_t c[4];
    uint8_t uc[4];
};

namespace pack32
{
    typedef float pvec3[3];

    uint32_t Vec2PackTexCoordsUV(const float (&in)[2])
    {
        return static_cast<uint32_t>(HalfFloat::ToHalf(in[1])) << 16 | HalfFloat::ToHalf(in[0]);
    }

    uint32_t Vec2PackTexCoordsVU(const float (&in)[2])
    {
        return static_cast<uint32_t>(HalfFloat::ToHalf(in[0])) << 16 | HalfFloat::ToHalf(in[1]);
    }

    float Vec3_Normalize(pvec3& vector)
    {
        float length = std::sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
        if (-length >= 0.0f)
            length = 1.0f;
        const auto lengthInv = 1.0f / length;
        vector[0] = lengthInv * vector[0];
        vector[1] = lengthInv * vector[1];
        vector[2] = lengthInv * vector[2];
        return length;
    }

    uint32_t Vec3PackUnitVecScaleBased(const float (&in)[3])
    {
        PackUtil32 testEncoding{};
        float normalized[3]{in[0], in[1], in[2]};
        float decoded[3];

        Vec3_Normalize(normalized);
        uint32_t out = 0u;
        auto bestDirError = 3.4028235e38f;
        auto bestLenError = 3.4028235e38f;
        testEncoding.uc[3] = 0u;
        do
        {
            const auto encodeScale = 32385.0f / (static_cast<float>(testEncoding.uc[3]) - -192.0f);
            testEncoding.c[0] = static_cast<int8_t>(normalized[0] * encodeScale + 127.5f);
            testEncoding.c[1] = static_cast<int8_t>(normalized[1] * encodeScale + 127.5f);
            testEncoding.c[2] = static_cast<int8_t>(normalized[2] * encodeScale + 127.5f);
            const auto decodeScale = (static_cast<float>(testEncoding.uc[3]) - -192.0f) / 32385.0f;
            decoded[0] = (static_cast<float>(testEncoding.uc[0]) - 127.0f) * decodeScale;
            decoded[1] = (static_cast<float>(testEncoding.uc[1]) - 127.0f) * decodeScale;
            decoded[2] = (static_cast<float>(testEncoding.uc[2]) - 127.0f) * decodeScale;
            const auto v2 = Vec3_Normalize(decoded) - 1.0f;
            const auto lenError = std::abs(v2);
            if (lenError < 0.001f)
            {
                const auto dirError = std::abs(decoded[0] * normalized[0] + decoded[1] * normalized[1] + decoded[2] * normalized[2] - 1.0f);
                if (bestDirError > dirError || bestDirError <= dirError && bestLenError > lenError)
                {
                    bestDirError = dirError;
                    bestLenError = lenError;
                    out = testEncoding.u;
                    if (lenError + dirError == 0.0f)
                        return out;
                }
            }
            ++testEncoding.c[3];
        } while (testEncoding.c[3]);

        return out;
    }

    uint32_t Vec3PackUnitVecThirdBased(const float (&in)[3])
    {
        // This is based on the game's reversed code, the original code may have made a bit more sense
        PackUtil32 x;
        x.f = (in[0] - -24624.0939334638f) * 0.0001218318939208984f;
        PackUtil32 y;
        y.f = (in[1] - -24624.0939334638f) * 0.0001218318939208984f;
        PackUtil32 z;
        z.f = (in[2] - -24624.0939334638f) * 0.0001218318939208984f;

        return (x.u & 0x3FF) | (y.u & 0x3FF) << 10u | (z.u & 0x3FF) << 20u;
    }

    uint32_t Vec4PackGfxColor(const float (&in)[4])
    {
        // clang-format off
        return static_cast<uint8_t>(std::clamp(in[0], 0.0f, 1.0f) * 255.0f)
            | static_cast<uint8_t>(std::clamp(in[1], 0.0f, 1.0f) * 255.0f) << 8
            | static_cast<uint8_t>(std::clamp(in[2], 0.0f, 1.0f) * 255.0f) << 16
            | static_cast<uint8_t>(std::clamp(in[3], 0.0f, 1.0f) * 255.0f) << 24;
        // clang-format on
    }

    void Vec2UnpackTexCoordsUV(const uint32_t in, float (&out)[2])
    {
        const auto inHiDw = static_cast<half_float_t>((in >> 16) & std::numeric_limits<uint16_t>::max());
        const auto inLoDw = static_cast<half_float_t>(in & std::numeric_limits<uint16_t>::max());

        out[0] = HalfFloat::ToFloat(inLoDw);
        out[1] = HalfFloat::ToFloat(inHiDw);
    }

    void Vec2UnpackTexCoordsVU(const uint32_t in, float (&out)[2])
    {
        const auto inHiDw = static_cast<half_float_t>((in >> 16) & std::numeric_limits<uint16_t>::max());
        const auto inLoDw = static_cast<half_float_t>(in & std::numeric_limits<uint16_t>::max());

        out[0] = HalfFloat::ToFloat(inHiDw);
        out[1] = HalfFloat::ToFloat(inLoDw);
    }

    void Vec3UnpackUnitVecScaleBased(const uint32_t in, float (&out)[3])
    {
        assert(out != nullptr);

        const PackUtil32 inUtil{in};
        const float decodeScale = (static_cast<float>(inUtil.uc[3]) - -192.0f) / 32385.0f;
        out[0] = (static_cast<float>(inUtil.uc[0]) + -127.0f) * decodeScale;
        out[1] = (static_cast<float>(inUtil.uc[1]) + -127.0f) * decodeScale;
        out[2] = (static_cast<float>(inUtil.uc[2]) + -127.0f) * decodeScale;
    }

    void Vec3UnpackUnitVecThirdBased(const uint32_t in, float (&out)[3])
    {
        // This is based on the game's reversed code, the original code may have made a bit more sense
        PackUtil32 v0{(in >> 0) & 0x3FF};
        PackUtil32 v1{(in >> 10) & 0x3FF};
        PackUtil32 v2{(in >> 20) & 0x3FF};

        v0.u = v0.u - 2 * (v0.u & 0x200) + 0x40400000;
        v1.u = v1.u - 2 * (v1.u & 0x200) + 0x40400000;
        v2.u = v2.u - 2 * (v2.u & 0x200) + 0x40400000;
        out[0] = (v0.f - 3.0f) * 8208.0312f;
        out[1] = (v1.f - 3.0f) * 8208.0312f;
        out[2] = (v2.f - 3.0f) * 8208.0312f;
    }

    void Vec4UnpackGfxColor(const uint32_t in, float (&out)[4])
    {
        out[0] = static_cast<float>(in & std::numeric_limits<uint8_t>::max()) / 255.0f;
        out[1] = static_cast<float>((in >> 8) & std::numeric_limits<uint8_t>::max()) / 255.0f;
        out[2] = static_cast<float>((in >> 16) & std::numeric_limits<uint8_t>::max()) / 255.0f;
        out[3] = static_cast<float>((in >> 24) & std::numeric_limits<uint8_t>::max()) / 255.0f;
    }

} // namespace pack32
