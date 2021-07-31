#include "Pack.h"

#include <cassert>

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
    // TODO
    return 0;
}

uint32_t Pack32::Vec3PackUnitVec(const float* in)
{
    // TODO
    return 0;
}

void Pack32::Vec2UnpackTexCoords(const uint32_t in, float* out)
{
    PackUtil32 packTemp{};

    const auto inHiDw = (in >> 16) & UINT16_MAX;
    const auto inLoDw = in & UINT16_MAX;

    if (inHiDw)
        packTemp.u = ((inHiDw << 16) & 0x80000000) | (((((inHiDw << 14) & 0xFFFC000)
            - (~(inHiDw << 14) & 0x10000000)) ^ 0x80000000) >> 1);
    else
        packTemp.f = 0.0f;
    out[0] = packTemp.f;

    if (inLoDw)
        packTemp.u = ((inLoDw << 16) & 0x80000000) | (((((inLoDw << 14) & 0xFFFC000)
            - (~(inLoDw << 14) & 0x10000000)) ^ 0x80000000) >> 1);
    else
        packTemp.f = 0.0f;
    out[1] = packTemp.f;
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
