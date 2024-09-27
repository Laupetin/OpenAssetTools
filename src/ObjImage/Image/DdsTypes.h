#pragma once

#include <cstdint>

constexpr uint32_t MakeFourCc(const char ch0, const char ch1, const char ch2, const char ch3)
{
    return static_cast<uint32_t>(ch0) | static_cast<uint32_t>(ch1) << 8 | static_cast<uint32_t>(ch2) << 16 | static_cast<uint32_t>(ch3) << 24;
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

enum D3D10_RESOURCE_DIMENSION
{
    D3D10_RESOURCE_DIMENSION_UNKNOWN,
    D3D10_RESOURCE_DIMENSION_BUFFER,
    D3D10_RESOURCE_DIMENSION_TEXTURE1D,
    D3D10_RESOURCE_DIMENSION_TEXTURE2D,
    D3D10_RESOURCE_DIMENSION_TEXTURE3D
};

enum DDS_HEADER_DXT10_MISC
{
    DDS_RESOURCE_MISC_TEXTURECUBE = 0x4
};

enum DDS_HEADER_DXT10_MISC2
{
    DDS_ALPHA_MODE_UNKNOWN = 0x0,
    DDS_ALPHA_MODE_STRAIGHT = 0x1,
    DDS_ALPHA_MODE_PREMULTIPLIED = 0x2,
    DDS_ALPHA_MODE_OPAQUE = 0x3,
    DDS_ALPHA_MODE_CUSTOM = 0x4,
};

struct DDS_HEADER_DXT10
{
    oat::DXGI_FORMAT dxgiFormat;
    D3D10_RESOURCE_DIMENSION resourceDimension;
    uint32_t miscFlag;
    uint32_t arraySize;
    uint32_t miscFlags2;
};
