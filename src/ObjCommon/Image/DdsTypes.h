#pragma once

#include <cstdint>
#include <dxgiformat.h>

namespace dds
{
    constexpr uint32_t MakeFourCc(const char c0, const char c1, const char c2, const char c3)
    {
        return static_cast<uint32_t>(c0)
            | static_cast<uint32_t>(c1) << 8
            | static_cast<uint32_t>(c2) << 16
            | static_cast<uint32_t>(c3) << 24;
    }

    enum DDP_FLAGS
    {
        DDPF_ALPHAPIXELS = 0x1,
        DDPF_ALPHA = 0x2,
        DDPF_FOURCC = 0x4,
        DDPF_RGB = 0x40,
        DDPF_YUV = 0x200,
        DDPF_LUMINANCE = 0x20000
    };

    struct DDS_PIXELFORMAT
    {
        uint32_t dwSize;
        uint32_t dwFlags;
        uint32_t dwFourCC;
        uint32_t dwRGBBitCount;
        uint32_t dwRBitMask;
        uint32_t dwGBitMask;
        uint32_t dwBBitMask;
        uint32_t dwABitMask;
    };

    enum DDS_HEADER_FLAGS
    {
        DDSD_CAPS = 0x1,
        DDSD_HEIGHT = 0x2,
        DDSD_WIDTH = 0x4,
        DDSD_PITCH = 0x8,
        DDSD_PIXELFORMAT = 0x1000,
        DDSD_MIPMAPCOUNT = 0x20000,
        DDSD_LINEARSIZE = 0x80000,
        DDSD_DEPTH = 0x800000,
    };

    enum DDS_HEADER_CAPS
    {
        DDSCAPS_COMPLEX = 0x8,
        DDSCAPS_TEXTURE = 0x1000,
        DDSCAPS_MIPMAP = 0x400000,
    };

    enum DDS_HEADER_CAPS2
    {
        DDSCAPS2_CUBEMAP = 0x200,
        DDSCAPS2_CUBEMAP_POSITIVEX = 0x400,
        DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800,
        DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000,
        DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000,
        DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000,
        DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000,
        DDSCAPS2_VOLUME = 0x200000,
    };

    struct DDS_HEADER
    {
        uint32_t dwSize;
        uint32_t dwFlags;
        uint32_t dwHeight;
        uint32_t dwWidth;
        uint32_t dwPitchOrLinearSize;
        uint32_t dwDepth;
        uint32_t dwMipMapCount;
        uint32_t dwReserved1[11];
        DDS_PIXELFORMAT ddspf;
        uint32_t dwCaps;
        uint32_t dwCaps2;
        uint32_t dwCaps3;
        uint32_t dwCaps4;
        uint32_t dwReserved2;
    };

    enum DDS_DIMENSION
    {
        DDS_DIMENSION_TEXTURE1D = 0x2,
        DDS_DIMENSION_TEXTURE2D,
        DDS_DIMENSION_TEXTURE3D
    };

    enum DDS_RESOURCE_MISC
    {
        DDS_RESOURCE_MISC_TEXTURECUBE = 0x4
    };

    struct DDS_HEADER_DXT10
    {
        DXGI_FORMAT dxgiFormat;
        DDS_DIMENSION resourceDimension;
        uint32_t miscFlag;
        uint32_t arraySize;
        uint32_t miscFlags2;
    };
}
