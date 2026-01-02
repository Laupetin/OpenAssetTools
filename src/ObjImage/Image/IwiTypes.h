#pragma once

#include <cstdint>

namespace image
{
    enum class IwiVersion : std::uint8_t
    {
        // IW3
        IWI_6 = 6,
        // IW4, IW5
        IWI_8 = 8,
        // T5
        IWI_13 = 13,
        // T6
        IWI_27 = 27
    };

    struct CommonIwiMetaData
    {
        // Always high resolution
        bool m_no_picmip;
        bool m_streaming;
        bool m_clamp_u;
        bool m_clamp_v;
        bool m_dynamic;

        float m_gamma;
    };

    struct IwiVersionHeader
    {
        char tag[3];
        char version;
    };

    namespace iwi6
    {
        struct IwiHeader
        {
            int8_t format;
            int8_t flags;
            uint16_t dimensions[3];
            uint32_t fileSizeForPicmip[4];
        };

        enum class IwiFormat
        {
            IMG_FORMAT_INVALID = 0x0,
            IMG_FORMAT_BITMAP_RGBA = 0x1,
            IMG_FORMAT_BITMAP_RGB = 0x2,
            IMG_FORMAT_BITMAP_LUMINANCE_ALPHA = 0x3,
            IMG_FORMAT_BITMAP_LUMINANCE = 0x4,
            IMG_FORMAT_BITMAP_ALPHA = 0x5,
            IMG_FORMAT_WAVELET_RGBA = 0x6,
            IMG_FORMAT_WAVELET_RGB = 0x7,
            IMG_FORMAT_WAVELET_LUMINANCE_ALPHA = 0x8,
            IMG_FORMAT_WAVELET_LUMINANCE = 0x9,
            IMG_FORMAT_WAVELET_ALPHA = 0xA,
            IMG_FORMAT_DXT1 = 0xB,
            IMG_FORMAT_DXT3 = 0xC,
            IMG_FORMAT_DXT5 = 0xD,
            IMG_FORMAT_DXN = 0xE,

            IMG_FORMAT_COUNT
        };

        enum IwiFlags
        {
            IMG_FLAG_NOPICMIP = 1 << 0,
            IMG_FLAG_NOMIPMAPS = 1 << 1,
            IMG_FLAG_CUBEMAP = 1 << 2,
            IMG_FLAG_VOLMAP = 1 << 3,
            IMG_FLAG_STREAMING = 1 << 4,
            IMG_FLAG_LEGACY_NORMALS = 1 << 5,
            IMG_FLAG_CLAMP_U = 1 << 6,
            IMG_FLAG_CLAMP_V = 1 << 7,
            IMG_FLAG_DYNAMIC = 1 << 16,
            IMG_FLAG_RENDER_TARGET = 1 << 17,
            IMG_FLAG_SYSTEMMEM = 1 << 18
        };
    } // namespace iwi6

    namespace iwi8
    {
        struct IwiHeader
        {
            uint32_t flags;
            int8_t format;
            int8_t unused;
            uint16_t dimensions[3];
            uint32_t fileSizeForPicmip[4];
        };

        enum class IwiFormat
        {
            IMG_FORMAT_INVALID = 0x0,
            IMG_FORMAT_BITMAP_RGBA = 0x1,
            IMG_FORMAT_BITMAP_RGB = 0x2,
            IMG_FORMAT_BITMAP_LUMINANCE_ALPHA = 0x3,
            IMG_FORMAT_BITMAP_LUMINANCE = 0x4,
            IMG_FORMAT_BITMAP_ALPHA = 0x5,
            IMG_FORMAT_WAVELET_RGBA = 0x6,
            IMG_FORMAT_WAVELET_RGB = 0x7,
            IMG_FORMAT_WAVELET_LUMINANCE_ALPHA = 0x8,
            IMG_FORMAT_WAVELET_LUMINANCE = 0x9,
            IMG_FORMAT_WAVELET_ALPHA = 0xA,
            IMG_FORMAT_DXT1 = 0xB,
            IMG_FORMAT_DXT3 = 0xC,
            IMG_FORMAT_DXT5 = 0xD,
            IMG_FORMAT_DXN = 0xE,
            IMG_FORMAT_DXT3A_AS_LUMINANCE = 0xF,
            IMG_FORMAT_DXT5A_AS_LUMINANCE = 0x10,
            IMG_FORMAT_DXT3A_AS_ALPHA = 0x11,
            IMG_FORMAT_DXT5A_AS_ALPHA = 0x12,
            IMG_FORMAT_DXT1_AS_LUMINANCE_ALPHA = 0x13,
            IMG_FORMAT_DXN_AS_LUMINANCE_ALPHA = 0x14,
            IMG_FORMAT_DXT1_AS_LUMINANCE = 0x15,
            IMG_FORMAT_DXT1_AS_ALPHA = 0x16,

            IMG_FORMAT_COUNT
        };

        enum IwiFlags
        {
            IMG_FLAG_NOPICMIP = 1 << 0,
            IMG_FLAG_NOMIPMAPS = 1 << 1,
            IMG_FLAG_STREAMING = 1 << 2,
            IMG_FLAG_LEGACY_NORMALS = 1 << 3,
            IMG_FLAG_CLAMP_U = 1 << 4,
            IMG_FLAG_CLAMP_V = 1 << 5,
            IMG_FLAG_ALPHA_WEIGHTED_COLORS = 1 << 6,
            IMG_FLAG_DXTC_APPROX_WEIGHTS = 1 << 7,
            IMG_FLAG_GAMMA_NONE = 0,
            IMG_FLAG_GAMMA_SRGB = 1 << 8,
            IMG_FLAG_GAMMA_PWL = 1 << 9,
            IMG_FLAG_GAMMA_2 = IMG_FLAG_GAMMA_SRGB | IMG_FLAG_GAMMA_PWL,
            IMG_FLAG_MAPTYPE_2D = 0,
            IMG_FLAG_MAPTYPE_CUBE = 1 << 16,
            IMG_FLAG_MAPTYPE_3D = 1 << 17,
            IMG_FLAG_MAPTYPE_1D = IMG_FLAG_MAPTYPE_CUBE | IMG_FLAG_MAPTYPE_3D,
            IMG_FLAG_MAPTYPE_MASK = IMG_FLAG_MAPTYPE_2D | IMG_FLAG_MAPTYPE_CUBE | IMG_FLAG_MAPTYPE_3D | IMG_FLAG_MAPTYPE_1D,
            IMG_FLAG_NORMALMAP = 1 << 18,
            IMG_FLAG_INTENSITY_TO_ALPHA = 1 << 19,
            IMG_FLAG_DYNAMIC = 1 << 24,
            IMG_FLAG_RENDER_TARGET = 1 << 25,
            IMG_FLAG_SYSTEMMEM = 1 << 26
        };
    } // namespace iwi8

    namespace iwi13
    {
        struct IwiHeader
        {
            int8_t format;
            int8_t flags;
            uint16_t dimensions[3];
            float gamma;
            uint32_t fileSizeForPicmip[8];
        };

        enum class IwiFormat
        {
            IMG_FORMAT_INVALID = 0x0,
            IMG_FORMAT_BITMAP_RGBA = 0x1,
            IMG_FORMAT_BITMAP_RGB = 0x2,
            IMG_FORMAT_BITMAP_LUMINANCE_ALPHA = 0x3,
            IMG_FORMAT_BITMAP_LUMINANCE = 0x4,
            IMG_FORMAT_BITMAP_ALPHA = 0x5,
            IMG_FORMAT_WAVELET_RGBA = 0x6,
            IMG_FORMAT_WAVELET_RGB = 0x7,
            IMG_FORMAT_WAVELET_LUMINANCE_ALPHA = 0x8,
            IMG_FORMAT_WAVELET_LUMINANCE = 0x9,
            IMG_FORMAT_WAVELET_ALPHA = 0xA,
            IMG_FORMAT_DXT1 = 0xB,
            IMG_FORMAT_DXT3 = 0xC,
            IMG_FORMAT_DXT5 = 0xD,
            IMG_FORMAT_DXN = 0xE,
            IMG_FORMAT_BITMAP_RGB565 = 0xF,
            IMG_FORMAT_BITMAP_RGB5A3 = 0x10,
            IMG_FORMAT_BITMAP_C8 = 0x11,
            IMG_FORMAT_BITMAP_RGBA8 = 0x12,
            IMG_FORMAT_A16B16G16R16F = 0x13,
            IMG_FORMAT_COUNT = 0x14,
        };

        enum IwiFlags
        {
            IMG_FLAG_NOPICMIP = 1 << 0,
            IMG_FLAG_NOMIPMAPS = 1 << 1,
            IMG_FLAG_CUBEMAP = 1 << 2,
            IMG_FLAG_VOLMAP = 1 << 3,
            IMG_FLAG_STREAMING = 1 << 4,
            IMG_FLAG_LEGACY_NORMALS = 1 << 5,
            IMG_FLAG_CLAMP_U = 1 << 6,
            IMG_FLAG_CLAMP_V = 1 << 7,
            IMG_FLAG_FORCE_SYSTEM = 1 << 8,
            IMG_FLAG_DYNAMIC = 1 << 16,
            IMG_FLAG_RENDER_TARGET = 1 << 17,
            IMG_FLAG_SYSTEMMEM = 1 << 18,
        };

    } // namespace iwi13

    namespace iwi27
    {
        struct IwiHeader
        {
            int8_t format;
            int8_t flags;
            uint16_t dimensions[3];
            float gamma;
            int8_t maxGlossForMip[16];
            uint32_t fileSizeForPicmip[8];
        };

        enum class IwiFormat
        {
            IMG_FORMAT_INVALID = 0x0,
            IMG_FORMAT_BITMAP_RGBA = 0x1,
            IMG_FORMAT_BITMAP_RGB = 0x2,
            IMG_FORMAT_BITMAP_LUMINANCE_ALPHA = 0x3,
            IMG_FORMAT_BITMAP_LUMINANCE = 0x4,
            IMG_FORMAT_BITMAP_ALPHA = 0x5,
            IMG_FORMAT_WAVELET_RGBA = 0x6,
            IMG_FORMAT_WAVELET_RGB = 0x7,
            IMG_FORMAT_WAVELET_LUMINANCE_ALPHA = 0x8,
            IMG_FORMAT_WAVELET_LUMINANCE = 0x9,
            IMG_FORMAT_WAVELET_ALPHA = 0xA,
            IMG_FORMAT_DXT1 = 0xB,
            IMG_FORMAT_DXT3 = 0xC,
            IMG_FORMAT_DXT5 = 0xD,
            IMG_FORMAT_DXN = 0xE,
            IMG_FORMAT_BITMAP_RGB565 = 0xF,
            IMG_FORMAT_BITMAP_RGB5A3 = 0x10,
            IMG_FORMAT_BITMAP_C8 = 0x11,
            IMG_FORMAT_BITMAP_RGBA8 = 0x12,
            IMG_FORMAT_A16B16G16R16F = 0x13,
            IMG_FORMAT_COUNT,
        };

        enum IwiFlags
        {
            IMG_FLAG_NOPICMIP = 1 << 0,
            IMG_FLAG_NOMIPMAPS = 1 << 1,
            IMG_FLAG_CUBEMAP = 1 << 2,
            IMG_FLAG_VOLMAP = 1 << 3,
            IMG_FLAG_STREAMING = 1 << 4,
            IMG_FLAG_CLAMP_U = 1 << 6,
            IMG_FLAG_CLAMP_V = 1 << 7,
            IMG_FLAG_FORCE_SYSTEM = 1 << 8,
            IMG_FLAG_DYNAMIC = 1 << 16,
            IMG_FLAG_RENDER_TARGET = 1 << 17,
            IMG_FLAG_MULTISAMPLE = 1 << 18,
        };

    } // namespace iwi27
} // namespace image
