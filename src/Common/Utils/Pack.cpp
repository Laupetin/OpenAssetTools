#include "Pack.h"

#include <algorithm>
#include <cassert>

#include "HalfFloat.h"

union PackUtil32
{
    uint32_t u;
    int32_t i;
    float f;
    int8_t c[4];
    uint8_t uc[4];
};

uint32_t Pack32::Vec2PackTexCoords(const float* in)
{
    return static_cast<uint32_t>(HalfFloat::ToHalf(in[0])) << 16
        | HalfFloat::ToHalf(in[1]);
}

uint32_t Pack32::Vec3PackUnitVec(const float* in)
{
    // TODO
    return 0;
}

uint32_t Pack32::Vec4PackGfxColor(const float* in)
{
    return static_cast<uint8_t>(std::clamp(in[0], 0.0f, 1.0f) * 255.0f)
        | static_cast<uint8_t>(std::clamp(in[1], 0.0f, 1.0f) * 255.0f) << 8
        | static_cast<uint8_t>(std::clamp(in[2], 0.0f, 1.0f) * 255.0f) << 16
        | static_cast<uint8_t>(std::clamp(in[3], 0.0f, 1.0f) * 255.0f) << 24;
}

void Pack32::Vec2UnpackTexCoords(const uint32_t in, float* out)
{
    PackUtil32 packTemp{};

    const auto inHiDw = static_cast<half_float_t>((in >> 16) & UINT16_MAX);
    const auto inLoDw = static_cast<half_float_t>(in & UINT16_MAX);

    out[0] = HalfFloat::ToFloat(inHiDw);
    out[1] = HalfFloat::ToFloat(inLoDw);
}

void Pack32::Vec3UnpackUnitVec(const uint32_t in, float* out)
{
    assert(out != nullptr);

    PackUtil32 _in{in};
    const float decodeScale = (static_cast<float>(_in.uc[3]) - -192.0f) / 32385.0f;
    out[0] = (static_cast<float>(_in.uc[0]) + -127.0f) * decodeScale;
    out[1] = (static_cast<float>(_in.uc[1]) + -127.0f) * decodeScale;
    out[2] = (static_cast<float>(_in.uc[2]) + -127.0f) * decodeScale;
}

void Pack32::Vec4UnpackGfxColor(uint32_t in, float* out)
{
    out[0] = static_cast<float>(in & UINT8_MAX) / 255.0f;
    out[1] = static_cast<float>((in >> 8) & UINT8_MAX) / 255.0f;
    out[2] = static_cast<float>((in >> 16) & UINT8_MAX) / 255.0f;
    out[3] = static_cast<float>((in >> 24) & UINT8_MAX) / 255.0f;
}
