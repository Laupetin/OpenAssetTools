#include "DecompressorBc2.h"

#include "Utils/Alignment.h"

#include <cassert>
#include <limits>

namespace
{
    constexpr auto BC2_BLOCK_PIXELS = 4u;
    constexpr unsigned BC2_BLUE_MASK = 0x1F;
    constexpr unsigned BC2_BLUE_SHIFT = 0;
    constexpr unsigned BC2_BLUE_MAX = 0x1F;
    constexpr unsigned BC2_GREEN_MASK = 0x7E0;
    constexpr unsigned BC2_GREEN_SHIFT = 5;
    constexpr unsigned BC2_GREEN_MAX = 0x3F;
    constexpr unsigned BC2_RED_MASK = 0xF800;
    constexpr unsigned BC2_RED_SHIFT = 11;
    constexpr unsigned BC2_RED_MAX = 0x1F;
    constexpr auto BC2_COLOR_CHANNELS_SIZE = 8u;
    constexpr auto BC2_ALPHA_CHANNEL_SIZE = 8u;
    constexpr auto BC2_BLOCK_SIZE = BC2_COLOR_CHANNELS_SIZE + BC2_ALPHA_CHANNEL_SIZE;

    void SetupColorTables(const uint8_t* in, uint8_t (&colorTableRed)[4], uint8_t (&colorTableGreen)[4], uint8_t (&colorTableBlue)[4])
    {
#define EXTRACT_COLOR_CHANNEL_BC2(color, mask, shift, colMax)                                                                                                  \
    static_cast<uint8_t>(static_cast<uint16_t>(((color) & (mask)) >> (shift)) * std::numeric_limits<uint8_t>::max() / (colMax))
#define INTERPOLATE_BC2(colorTable, val0, val1)                                                                                                                \
    static_cast<uint8_t>(                                                                                                                                      \
        (static_cast<uint16_t>(val0) * static_cast<uint16_t>((colorTable)[0]) + static_cast<uint16_t>(val1) * static_cast<uint16_t>((colorTable)[1]))          \
        / ((val0) + (val1)))

        const auto color0 = static_cast<uint16_t>(in[0 + BC2_ALPHA_CHANNEL_SIZE] | (in[1 + BC2_ALPHA_CHANNEL_SIZE] << 8));
        const auto color1 = static_cast<uint16_t>(in[2 + BC2_ALPHA_CHANNEL_SIZE] | (in[3 + BC2_ALPHA_CHANNEL_SIZE] << 8));

        colorTableRed[0] = EXTRACT_COLOR_CHANNEL_BC2(color0, BC2_RED_MASK, BC2_RED_SHIFT, BC2_RED_MAX);
        colorTableRed[1] = EXTRACT_COLOR_CHANNEL_BC2(color1, BC2_RED_MASK, BC2_RED_SHIFT, BC2_RED_MAX);
        colorTableRed[2] = INTERPOLATE_BC2(colorTableRed, 2, 1);
        colorTableRed[3] = INTERPOLATE_BC2(colorTableRed, 1, 2);

        colorTableGreen[0] = EXTRACT_COLOR_CHANNEL_BC2(color0, BC2_GREEN_MASK, BC2_GREEN_SHIFT, BC2_GREEN_MAX);
        colorTableGreen[1] = EXTRACT_COLOR_CHANNEL_BC2(color1, BC2_GREEN_MASK, BC2_GREEN_SHIFT, BC2_GREEN_MAX);
        colorTableGreen[2] = INTERPOLATE_BC2(colorTableGreen, 2, 1);
        colorTableGreen[3] = INTERPOLATE_BC2(colorTableGreen, 1, 2);

        colorTableBlue[0] = EXTRACT_COLOR_CHANNEL_BC2(color0, BC2_BLUE_MASK, BC2_BLUE_SHIFT, BC2_BLUE_MAX);
        colorTableBlue[1] = EXTRACT_COLOR_CHANNEL_BC2(color1, BC2_BLUE_MASK, BC2_BLUE_SHIFT, BC2_BLUE_MAX);
        colorTableBlue[2] = INTERPOLATE_BC2(colorTableBlue, 2, 1);
        colorTableBlue[3] = INTERPOLATE_BC2(colorTableBlue, 1, 2);

#undef INTERPOLATE_BC2
#undef EXTRACT_COLOR_CHANNEL_BC2
    }

    void DecompressBlock(const uint8_t* in,
                         uint8_t* out,
                         const unsigned outPitch,
                         const unsigned outPixelSize,
                         const unsigned outOffsetR,
                         const unsigned outOffsetG,
                         const unsigned outOffsetB,
                         const unsigned outOffsetA)
    {
        uint8_t colorTableRed[4];
        uint8_t colorTableGreen[4];
        uint8_t colorTableBlue[4];
        SetupColorTables(in, colorTableRed, colorTableGreen, colorTableBlue);

        const uint16_t dataAlpha0 = static_cast<uint16_t>(in[0] | static_cast<uint32_t>(in[1] << 8u));
        const uint16_t dataAlpha1 = static_cast<uint16_t>(in[2] | static_cast<uint32_t>(in[3] << 8u));
        const uint16_t dataAlpha2 = static_cast<uint16_t>(in[4] | static_cast<uint32_t>(in[5] << 8u));
        const uint16_t dataAlpha3 = static_cast<uint16_t>(in[6] | static_cast<uint32_t>(in[7] << 8u));
        const uint32_t dataColor0 = in[BC2_ALPHA_CHANNEL_SIZE + 4];
        const uint32_t dataColor1 = in[BC2_ALPHA_CHANNEL_SIZE + 5];
        const uint32_t dataColor2 = in[BC2_ALPHA_CHANNEL_SIZE + 6];
        const uint32_t dataColor3 = in[BC2_ALPHA_CHANNEL_SIZE + 7];

        out[0 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataColor0 & (0x3 << 0)) >> 0];
        out[0 * outPitch + 0 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor0 & (0x3 << 0)) >> 0];
        out[0 * outPitch + 0 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor0 & (0x3 << 0)) >> 0];
        out[0 * outPitch + 0 * outPixelSize + outOffsetA] = ((dataAlpha0 & 0xF) >> 0) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[0 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataColor0 & (0x3 << 2)) >> 2];
        out[0 * outPitch + 1 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor0 & (0x3 << 2)) >> 2];
        out[0 * outPitch + 1 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor0 & (0x3 << 2)) >> 2];
        out[0 * outPitch + 1 * outPixelSize + outOffsetA] = ((dataAlpha0 & 0xF0) >> 4) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[0 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataColor0 & (0x3 << 4)) >> 4];
        out[0 * outPitch + 2 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor0 & (0x3 << 4)) >> 4];
        out[0 * outPitch + 2 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor0 & (0x3 << 4)) >> 4];
        out[0 * outPitch + 2 * outPixelSize + outOffsetA] = ((dataAlpha0 & 0xF00) >> 8) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[0 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataColor0 & (0x3 << 6)) >> 6];
        out[0 * outPitch + 3 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor0 & (0x3 << 6)) >> 6];
        out[0 * outPitch + 3 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor0 & (0x3 << 6)) >> 6];
        out[0 * outPitch + 3 * outPixelSize + outOffsetA] = ((dataAlpha0 & 0xF000) >> 12) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[1 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataColor1 & (0x3 << 0)) >> 0];
        out[1 * outPitch + 0 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor1 & (0x3 << 0)) >> 0];
        out[1 * outPitch + 0 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor1 & (0x3 << 0)) >> 0];
        out[1 * outPitch + 0 * outPixelSize + outOffsetA] = ((dataAlpha1 & 0xF) >> 0) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[1 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataColor1 & (0x3 << 2)) >> 2];
        out[1 * outPitch + 1 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor1 & (0x3 << 2)) >> 2];
        out[1 * outPitch + 1 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor1 & (0x3 << 2)) >> 2];
        out[1 * outPitch + 1 * outPixelSize + outOffsetA] = ((dataAlpha1 & 0xF0) >> 4) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[1 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataColor1 & (0x3 << 4)) >> 4];
        out[1 * outPitch + 2 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor1 & (0x3 << 4)) >> 4];
        out[1 * outPitch + 2 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor1 & (0x3 << 4)) >> 4];
        out[1 * outPitch + 2 * outPixelSize + outOffsetA] = ((dataAlpha1 & 0xF00) >> 8) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[1 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataColor1 & (0x3 << 6)) >> 6];
        out[1 * outPitch + 3 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor1 & (0x3 << 6)) >> 6];
        out[1 * outPitch + 3 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor1 & (0x3 << 6)) >> 6];
        out[1 * outPitch + 3 * outPixelSize + outOffsetA] = ((dataAlpha1 & 0xF000) >> 12) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[2 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataColor2 & (0x3 << 0)) >> 0];
        out[2 * outPitch + 0 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor2 & (0x3 << 0)) >> 0];
        out[2 * outPitch + 0 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor2 & (0x3 << 0)) >> 0];
        out[2 * outPitch + 0 * outPixelSize + outOffsetA] = ((dataAlpha2 & 0xF) >> 0) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[2 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataColor2 & (0x3 << 2)) >> 2];
        out[2 * outPitch + 1 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor2 & (0x3 << 2)) >> 2];
        out[2 * outPitch + 1 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor2 & (0x3 << 2)) >> 2];
        out[2 * outPitch + 1 * outPixelSize + outOffsetA] = ((dataAlpha2 & 0xF0) >> 4) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[2 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataColor2 & (0x3 << 4)) >> 4];
        out[2 * outPitch + 2 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor2 & (0x3 << 4)) >> 4];
        out[2 * outPitch + 2 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor2 & (0x3 << 4)) >> 4];
        out[2 * outPitch + 2 * outPixelSize + outOffsetA] = ((dataAlpha2 & 0xF00) >> 8) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[2 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataColor2 & (0x3 << 6)) >> 6];
        out[2 * outPitch + 3 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor2 & (0x3 << 6)) >> 6];
        out[2 * outPitch + 3 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor2 & (0x3 << 6)) >> 6];
        out[2 * outPitch + 3 * outPixelSize + outOffsetA] = ((dataAlpha2 & 0xF000) >> 12) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[3 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataColor3 & (0x3 << 0)) >> 0];
        out[3 * outPitch + 0 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor3 & (0x3 << 0)) >> 0];
        out[3 * outPitch + 0 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor3 & (0x3 << 0)) >> 0];
        out[3 * outPitch + 0 * outPixelSize + outOffsetA] = ((dataAlpha3 & 0xF) >> 0) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[3 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataColor3 & (0x3 << 2)) >> 2];
        out[3 * outPitch + 1 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor3 & (0x3 << 2)) >> 2];
        out[3 * outPitch + 1 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor3 & (0x3 << 2)) >> 2];
        out[3 * outPitch + 1 * outPixelSize + outOffsetA] = ((dataAlpha3 & 0xF0) >> 4) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[3 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataColor3 & (0x3 << 4)) >> 4];
        out[3 * outPitch + 2 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor3 & (0x3 << 4)) >> 4];
        out[3 * outPitch + 2 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor3 & (0x3 << 4)) >> 4];
        out[3 * outPitch + 2 * outPixelSize + outOffsetA] = ((dataAlpha3 & 0xF00) >> 8) * std::numeric_limits<uint8_t>::max() / 0xF;

        out[3 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataColor3 & (0x3 << 6)) >> 6];
        out[3 * outPitch + 3 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor3 & (0x3 << 6)) >> 6];
        out[3 * outPitch + 3 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor3 & (0x3 << 6)) >> 6];
        out[3 * outPitch + 3 * outPixelSize + outOffsetA] = ((dataAlpha3 & 0xF000) >> 12) * std::numeric_limits<uint8_t>::max() / 0xF;
    }

    void DecompressBlockEdge(const uint8_t* in,
                             uint8_t* out,
                             const unsigned width,
                             const unsigned height,
                             const unsigned outPitch,
                             const unsigned outPixelSize,
                             const unsigned outOffsetR,
                             const unsigned outOffsetG,
                             const unsigned outOffsetB,
                             const unsigned outOffsetA)
    {
        assert(width <= BC2_BLOCK_PIXELS);
        assert(height <= BC2_BLOCK_PIXELS);

        uint8_t colorTableRed[4];
        uint8_t colorTableGreen[4];
        uint8_t colorTableBlue[4];
        SetupColorTables(in, colorTableRed, colorTableGreen, colorTableBlue);

        const uint16_t dataAlpha0 = static_cast<uint16_t>(in[0] | static_cast<uint32_t>(in[1] << 8u));
        const uint16_t dataAlpha1 = static_cast<uint16_t>(in[2] | static_cast<uint32_t>(in[3] << 8u));
        const uint16_t dataAlpha2 = static_cast<uint16_t>(in[4] | static_cast<uint32_t>(in[5] << 8u));
        const uint16_t dataAlpha3 = static_cast<uint16_t>(in[6] | static_cast<uint32_t>(in[7] << 8u));
        const uint8_t dataColor0 = in[BC2_ALPHA_CHANNEL_SIZE + 4];
        const uint8_t dataColor1 = in[BC2_ALPHA_CHANNEL_SIZE + 5];
        const uint8_t dataColor2 = in[BC2_ALPHA_CHANNEL_SIZE + 6];
        const uint8_t dataColor3 = in[BC2_ALPHA_CHANNEL_SIZE + 7];

        const uint8_t pixelsRed[]{
            colorTableRed[(dataColor0 & (0x3 << 0)) >> 0],
            colorTableRed[(dataColor0 & (0x3 << 2)) >> 2],
            colorTableRed[(dataColor0 & (0x3 << 4)) >> 4],
            colorTableRed[(dataColor0 & (0x3 << 6)) >> 6],
            colorTableRed[(dataColor1 & (0x3 << 0)) >> 0],
            colorTableRed[(dataColor1 & (0x3 << 2)) >> 2],
            colorTableRed[(dataColor1 & (0x3 << 4)) >> 4],
            colorTableRed[(dataColor1 & (0x3 << 6)) >> 6],
            colorTableRed[(dataColor2 & (0x3 << 0)) >> 0],
            colorTableRed[(dataColor2 & (0x3 << 2)) >> 2],
            colorTableRed[(dataColor2 & (0x3 << 4)) >> 4],
            colorTableRed[(dataColor2 & (0x3 << 6)) >> 6],
            colorTableRed[(dataColor3 & (0x3 << 0)) >> 0],
            colorTableRed[(dataColor3 & (0x3 << 2)) >> 2],
            colorTableRed[(dataColor3 & (0x3 << 4)) >> 4],
            colorTableRed[(dataColor3 & (0x3 << 6)) >> 6],
        };
        static_assert(std::extent_v<decltype(pixelsRed)> == BC2_BLOCK_PIXELS * BC2_BLOCK_PIXELS);

        const uint8_t pixelsGreen[]{
            colorTableGreen[(dataColor0 & (0x3 << 0)) >> 0],
            colorTableGreen[(dataColor0 & (0x3 << 2)) >> 2],
            colorTableGreen[(dataColor0 & (0x3 << 4)) >> 4],
            colorTableGreen[(dataColor0 & (0x3 << 6)) >> 6],
            colorTableGreen[(dataColor1 & (0x3 << 0)) >> 0],
            colorTableGreen[(dataColor1 & (0x3 << 2)) >> 2],
            colorTableGreen[(dataColor1 & (0x3 << 4)) >> 4],
            colorTableGreen[(dataColor1 & (0x3 << 6)) >> 6],
            colorTableGreen[(dataColor2 & (0x3 << 0)) >> 0],
            colorTableGreen[(dataColor2 & (0x3 << 2)) >> 2],
            colorTableGreen[(dataColor2 & (0x3 << 4)) >> 4],
            colorTableGreen[(dataColor2 & (0x3 << 6)) >> 6],
            colorTableGreen[(dataColor3 & (0x3 << 0)) >> 0],
            colorTableGreen[(dataColor3 & (0x3 << 2)) >> 2],
            colorTableGreen[(dataColor3 & (0x3 << 4)) >> 4],
            colorTableGreen[(dataColor3 & (0x3 << 6)) >> 6],
        };
        static_assert(std::extent_v<decltype(pixelsGreen)> == BC2_BLOCK_PIXELS * BC2_BLOCK_PIXELS);

        const uint8_t pixelsBlue[]{
            colorTableBlue[(dataColor0 & (0x3 << 0)) >> 0],
            colorTableBlue[(dataColor0 & (0x3 << 2)) >> 2],
            colorTableBlue[(dataColor0 & (0x3 << 4)) >> 4],
            colorTableBlue[(dataColor0 & (0x3 << 6)) >> 6],
            colorTableBlue[(dataColor1 & (0x3 << 0)) >> 0],
            colorTableBlue[(dataColor1 & (0x3 << 2)) >> 2],
            colorTableBlue[(dataColor1 & (0x3 << 4)) >> 4],
            colorTableBlue[(dataColor1 & (0x3 << 6)) >> 6],
            colorTableBlue[(dataColor2 & (0x3 << 0)) >> 0],
            colorTableBlue[(dataColor2 & (0x3 << 2)) >> 2],
            colorTableBlue[(dataColor2 & (0x3 << 4)) >> 4],
            colorTableBlue[(dataColor2 & (0x3 << 6)) >> 6],
            colorTableBlue[(dataColor3 & (0x3 << 0)) >> 0],
            colorTableBlue[(dataColor3 & (0x3 << 2)) >> 2],
            colorTableBlue[(dataColor3 & (0x3 << 4)) >> 4],
            colorTableBlue[(dataColor3 & (0x3 << 6)) >> 6],
        };
        static_assert(std::extent_v<decltype(pixelsBlue)> == BC2_BLOCK_PIXELS * BC2_BLOCK_PIXELS);

        const uint8_t pixelsAlpha[]{
            static_cast<uint8_t>(((dataAlpha0 & 0xF) >> 0) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha0 & 0xF0) >> 4) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha0 & 0xF00) >> 8) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha0 & 0xF000) >> 12) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha1 & 0xF) >> 0) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha1 & 0xF0) >> 4) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha1 & 0xF00) >> 8) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha1 & 0xF000) >> 12) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha2 & 0xF) >> 0) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha2 & 0xF0) >> 4) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha2 & 0xF00) >> 8) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha2 & 0xF000) >> 12) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha3 & 0xF) >> 0) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha3 & 0xF0) >> 4) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha3 & 0xF00) >> 8) * std::numeric_limits<uint8_t>::max() / 0xF),
            static_cast<uint8_t>(((dataAlpha3 & 0xF000) >> 12) * std::numeric_limits<uint8_t>::max() / 0xF),
        };
        static_assert(std::extent_v<decltype(pixelsAlpha)> == BC2_BLOCK_PIXELS * BC2_BLOCK_PIXELS);

        for (auto curHeight = 0u; curHeight < height; curHeight++)
        {
            for (auto curWidth = 0u; curWidth < width; curWidth++)
            {
                out[curHeight * outPitch + curWidth * outPixelSize + outOffsetR] = pixelsRed[curHeight * BC2_BLOCK_PIXELS + curWidth];
                out[curHeight * outPitch + curWidth * outPixelSize + outOffsetG] = pixelsGreen[curHeight * BC2_BLOCK_PIXELS + curWidth];
                out[curHeight * outPitch + curWidth * outPixelSize + outOffsetB] = pixelsBlue[curHeight * BC2_BLOCK_PIXELS + curWidth];
                out[curHeight * outPitch + curWidth * outPixelSize + outOffsetA] = pixelsAlpha[curHeight * BC2_BLOCK_PIXELS + curWidth];
            }
        }
    }
} // namespace

namespace image
{
    std::unique_ptr<Texture> DecompressorBc2::Decompress(const Texture& input, const ImageFormat* targetFormat)
    {
        assert(targetFormat->GetType() == ImageFormatType::UNSIGNED);
        if (targetFormat->GetType() != ImageFormatType::UNSIGNED)
            return nullptr;

        const auto* unsignedTargetFormat = dynamic_cast<const ImageFormatUnsigned*>(targetFormat);

        const auto outPixelSize = unsignedTargetFormat->m_bits_per_pixel / 8u;

        // Only support formats with byte aligned channels
        const auto redByteAligned = unsignedTargetFormat->m_r_size == 8 && (unsignedTargetFormat->m_r_offset % 8) == 0;
        const auto greenByteAligned = unsignedTargetFormat->m_g_size == 8 && (unsignedTargetFormat->m_g_offset % 8) == 0;
        const auto blueByteAligned = unsignedTargetFormat->m_b_size == 8 && (unsignedTargetFormat->m_b_offset % 8) == 0;
        const auto alphaByteAligned = unsignedTargetFormat->m_a_size == 8 && (unsignedTargetFormat->m_a_offset % 8) == 0;
        assert(redByteAligned);
        assert(greenByteAligned);
        assert(blueByteAligned);
        assert(alphaByteAligned);
        if (!redByteAligned || !greenByteAligned || !blueByteAligned || !alphaByteAligned)
            return nullptr;

        const auto outOffsetR = unsignedTargetFormat->m_r_offset / 8;
        const auto outOffsetG = unsignedTargetFormat->m_g_offset / 8;
        const auto outOffsetB = unsignedTargetFormat->m_b_offset / 8;
        const auto outOffsetA = unsignedTargetFormat->m_a_offset / 8;

        const auto width = input.GetWidth();
        const auto height = input.GetHeight();
        const auto depth = input.GetDepth();

        auto result = Texture::CreateForType(input.GetTextureType(), targetFormat, width, height, depth, input.HasMipMaps());
        result->Allocate();

        const auto faceCount = result->GetFaceCount();
        const auto mipCount = result->HasMipMaps() ? result->GetMipMapCount() : 1;
        assert(mipCount == input.HasMipMaps() ? input.GetMipMapCount() : 1);

        for (auto mipLevel = 0; mipLevel < mipCount; mipLevel++)
        {
            const auto mipWidth = std::max<unsigned>(width >> mipLevel, 1u);
            const auto mipHeight = std::max<unsigned>(height >> mipLevel, 1u);
            const auto mipDepth = std::max<unsigned>(depth >> mipLevel, 1u);
            const unsigned mipPitch = outPixelSize * mipWidth;
            assert(mipPitch == result->GetFormat()->GetPitch(mipLevel, width));

            for (auto face = 0; face < faceCount; face++)
            {
                const auto* bufferIn = input.GetBufferForMipLevel(mipLevel, face);
                auto* bufferOut = result->GetBufferForMipLevel(mipLevel, face);

                for (auto curDepth = 0u; curDepth < mipDepth; curDepth++)
                {
                    const auto fullBlocksWidth = utils::AlignToPrevious(mipWidth, BC2_BLOCK_PIXELS);
                    const auto fullBlocksHeight = utils::AlignToPrevious(mipHeight, BC2_BLOCK_PIXELS);

                    for (auto curHeight = 0u; curHeight < fullBlocksHeight; curHeight += BC2_BLOCK_PIXELS)
                    {
                        for (auto curWidth = 0u; curWidth < fullBlocksWidth; curWidth += BC2_BLOCK_PIXELS)
                        {
                            DecompressBlock(bufferIn, bufferOut, mipPitch, outPixelSize, outOffsetR, outOffsetG, outOffsetB, outOffsetA);
                            bufferIn += BC2_BLOCK_SIZE;
                            bufferOut += outPixelSize * BC2_BLOCK_PIXELS;
                        }

                        if (fullBlocksWidth < mipWidth)
                        {
                            const auto edgeBlockWidth = mipWidth - fullBlocksWidth;
                            DecompressBlockEdge(
                                bufferIn, bufferOut, edgeBlockWidth, BC2_BLOCK_PIXELS, mipPitch, outPixelSize, outOffsetR, outOffsetG, outOffsetB, outOffsetA);
                            bufferIn += BC2_BLOCK_SIZE;
                            bufferOut += outPixelSize * edgeBlockWidth;
                        }

                        bufferOut += mipPitch * (BC2_BLOCK_PIXELS - 1);
                    }

                    if (fullBlocksHeight < mipHeight)
                    {
                        const auto edgeBlockHeight = mipHeight - fullBlocksHeight;
                        for (auto curWidth = 0u; curWidth < fullBlocksWidth; curWidth += BC2_BLOCK_PIXELS)
                        {
                            DecompressBlockEdge(
                                bufferIn, bufferOut, BC2_BLOCK_PIXELS, edgeBlockHeight, mipPitch, outPixelSize, outOffsetR, outOffsetG, outOffsetB, outOffsetA);
                            bufferIn += BC2_BLOCK_SIZE;
                            bufferOut += outPixelSize * BC2_BLOCK_PIXELS;
                        }

                        if (fullBlocksWidth < mipWidth)
                        {
                            const auto edgeBlockWidth = mipWidth - fullBlocksWidth;
                            DecompressBlockEdge(
                                bufferIn, bufferOut, edgeBlockWidth, edgeBlockHeight, mipPitch, outPixelSize, outOffsetR, outOffsetG, outOffsetB, outOffsetA);
                            bufferIn += BC2_BLOCK_SIZE;
                            bufferOut += outPixelSize * edgeBlockWidth;
                        }

                        bufferOut += mipPitch * (BC2_BLOCK_PIXELS - 1);
                    }
                }
            }
        }

        return result;
    }
} // namespace image
