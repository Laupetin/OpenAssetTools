#include "DecompressorBc3.h"

#include "Utils/Alignment.h"

#include <cassert>
#include <limits>

namespace
{
    constexpr auto BC3_BLOCK_PIXELS = 4u;
    constexpr unsigned BC3_BLUE_MASK = 0x1F;
    constexpr unsigned BC3_BLUE_SHIFT = 0;
    constexpr unsigned BC3_BLUE_MAX = 0x1F;
    constexpr unsigned BC3_GREEN_MASK = 0x7E0;
    constexpr unsigned BC3_GREEN_SHIFT = 5;
    constexpr unsigned BC3_GREEN_MAX = 0x3F;
    constexpr unsigned BC3_RED_MASK = 0xF800;
    constexpr unsigned BC3_RED_SHIFT = 11;
    constexpr unsigned BC3_RED_MAX = 0x1F;
    constexpr auto BC3_COLOR_CHANNELS_SIZE = 8u;
    constexpr auto BC3_ALPHA_CHANNEL_SIZE = 8u;
    constexpr auto BC3_BLOCK_SIZE = BC3_COLOR_CHANNELS_SIZE + BC3_ALPHA_CHANNEL_SIZE;

    void SetupColorTables(
        const uint8_t* in, uint8_t (&colorTableRed)[4], uint8_t (&colorTableGreen)[4], uint8_t (&colorTableBlue)[4], uint8_t (&colorTableAlpha)[8])
    {
#define EXTRACT_COLOR_CHANNEL_BC3(color, mask, shift, colMax)                                                                                                  \
    static_cast<uint8_t>(static_cast<uint16_t>(((color) & (mask)) >> (shift)) * std::numeric_limits<uint8_t>::max() / (colMax))
#define INTERPOLATE_BC3(colorTable, val0, val1)                                                                                                                \
    static_cast<uint8_t>(                                                                                                                                      \
        (static_cast<uint16_t>(val0) * static_cast<uint16_t>((colorTable)[0]) + static_cast<uint16_t>(val1) * static_cast<uint16_t>((colorTable)[1]))          \
        / ((val0) + (val1)))

        colorTableAlpha[0] = in[0];
        colorTableAlpha[1] = in[1];

        if (colorTableAlpha[0] > colorTableAlpha[1])
        {
            // 6 interpolated color values
            colorTableAlpha[2] = INTERPOLATE_BC3(colorTableAlpha, 6, 1);
            colorTableAlpha[3] = INTERPOLATE_BC3(colorTableAlpha, 5, 2);
            colorTableAlpha[4] = INTERPOLATE_BC3(colorTableAlpha, 4, 3);
            colorTableAlpha[5] = INTERPOLATE_BC3(colorTableAlpha, 3, 4);
            colorTableAlpha[6] = INTERPOLATE_BC3(colorTableAlpha, 2, 5);
            colorTableAlpha[7] = INTERPOLATE_BC3(colorTableAlpha, 1, 6);
        }
        else
        {
            // 4 interpolated color values
            colorTableAlpha[2] = INTERPOLATE_BC3(colorTableAlpha, 4, 1);
            colorTableAlpha[3] = INTERPOLATE_BC3(colorTableAlpha, 3, 2);
            colorTableAlpha[4] = INTERPOLATE_BC3(colorTableAlpha, 2, 3);
            colorTableAlpha[5] = INTERPOLATE_BC3(colorTableAlpha, 1, 4);
            colorTableAlpha[6] = std::numeric_limits<uint8_t>::min();
            colorTableAlpha[7] = std::numeric_limits<uint8_t>::max();
        }

        const auto color0 = static_cast<uint16_t>(in[0 + BC3_ALPHA_CHANNEL_SIZE] | (in[1 + BC3_ALPHA_CHANNEL_SIZE] << 8));
        const auto color1 = static_cast<uint16_t>(in[2 + BC3_ALPHA_CHANNEL_SIZE] | (in[3 + BC3_ALPHA_CHANNEL_SIZE] << 8));

        colorTableRed[0] = EXTRACT_COLOR_CHANNEL_BC3(color0, BC3_RED_MASK, BC3_RED_SHIFT, BC3_RED_MAX);
        colorTableRed[1] = EXTRACT_COLOR_CHANNEL_BC3(color1, BC3_RED_MASK, BC3_RED_SHIFT, BC3_RED_MAX);
        colorTableRed[2] = INTERPOLATE_BC3(colorTableRed, 2, 1);
        colorTableRed[3] = INTERPOLATE_BC3(colorTableRed, 1, 2);

        colorTableGreen[0] = EXTRACT_COLOR_CHANNEL_BC3(color0, BC3_GREEN_MASK, BC3_GREEN_SHIFT, BC3_GREEN_MAX);
        colorTableGreen[1] = EXTRACT_COLOR_CHANNEL_BC3(color1, BC3_GREEN_MASK, BC3_GREEN_SHIFT, BC3_GREEN_MAX);
        colorTableGreen[2] = INTERPOLATE_BC3(colorTableGreen, 2, 1);
        colorTableGreen[3] = INTERPOLATE_BC3(colorTableGreen, 1, 2);

        colorTableBlue[0] = EXTRACT_COLOR_CHANNEL_BC3(color0, BC3_BLUE_MASK, BC3_BLUE_SHIFT, BC3_BLUE_MAX);
        colorTableBlue[1] = EXTRACT_COLOR_CHANNEL_BC3(color1, BC3_BLUE_MASK, BC3_BLUE_SHIFT, BC3_BLUE_MAX);
        colorTableBlue[2] = INTERPOLATE_BC3(colorTableBlue, 2, 1);
        colorTableBlue[3] = INTERPOLATE_BC3(colorTableBlue, 1, 2);

#undef INTERPOLATE_BC3
#undef EXTRACT_COLOR_CHANNEL_BC3
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
        uint8_t colorTableAlpha[8];
        SetupColorTables(in, colorTableRed, colorTableGreen, colorTableBlue, colorTableAlpha);

        const uint32_t dataAlpha0 = in[2] | static_cast<uint32_t>(in[3] << 8u) | static_cast<uint32_t>(in[4] << 16u);
        const uint32_t dataAlpha1 = in[5] | static_cast<uint32_t>(in[6] << 8u) | static_cast<uint32_t>(in[7] << 16u);
        const uint8_t dataColor0 = in[BC3_ALPHA_CHANNEL_SIZE + 4];
        const uint8_t dataColor1 = in[BC3_ALPHA_CHANNEL_SIZE + 5];
        const uint8_t dataColor2 = in[BC3_ALPHA_CHANNEL_SIZE + 6];
        const uint8_t dataColor3 = in[BC3_ALPHA_CHANNEL_SIZE + 7];

        out[0 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataColor0 & (0x3 << 0)) >> 0];
        out[0 * outPitch + 0 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor0 & (0x3 << 0)) >> 0];
        out[0 * outPitch + 0 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor0 & (0x3 << 0)) >> 0];
        out[0 * outPitch + 0 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha0 & (0x7 << 0)) >> 0];

        out[0 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataColor0 & (0x3 << 2)) >> 2];
        out[0 * outPitch + 1 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor0 & (0x3 << 2)) >> 2];
        out[0 * outPitch + 1 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor0 & (0x3 << 2)) >> 2];
        out[0 * outPitch + 1 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha0 & (0x7 << 3)) >> 3];

        out[0 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataColor0 & (0x3 << 4)) >> 4];
        out[0 * outPitch + 2 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor0 & (0x3 << 4)) >> 4];
        out[0 * outPitch + 2 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor0 & (0x3 << 4)) >> 4];
        out[0 * outPitch + 2 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha0 & (0x7 << 6)) >> 6];

        out[0 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataColor0 & (0x3 << 6)) >> 6];
        out[0 * outPitch + 3 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor0 & (0x3 << 6)) >> 6];
        out[0 * outPitch + 3 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor0 & (0x3 << 6)) >> 6];
        out[0 * outPitch + 3 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha0 & (0x7 << 9)) >> 9];

        out[1 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataColor1 & (0x3 << 0)) >> 0];
        out[1 * outPitch + 0 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor1 & (0x3 << 0)) >> 0];
        out[1 * outPitch + 0 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor1 & (0x3 << 0)) >> 0];
        out[1 * outPitch + 0 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha0 & (0x7 << 12)) >> 12];

        out[1 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataColor1 & (0x3 << 2)) >> 2];
        out[1 * outPitch + 1 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor1 & (0x3 << 2)) >> 2];
        out[1 * outPitch + 1 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor1 & (0x3 << 2)) >> 2];
        out[1 * outPitch + 1 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha0 & (0x7 << 15)) >> 15];

        out[1 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataColor1 & (0x3 << 4)) >> 4];
        out[1 * outPitch + 2 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor1 & (0x3 << 4)) >> 4];
        out[1 * outPitch + 2 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor1 & (0x3 << 4)) >> 4];
        out[1 * outPitch + 2 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha0 & (0x7 << 18)) >> 18];

        out[1 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataColor1 & (0x3 << 6)) >> 6];
        out[1 * outPitch + 3 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor1 & (0x3 << 6)) >> 6];
        out[1 * outPitch + 3 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor1 & (0x3 << 6)) >> 6];
        out[1 * outPitch + 3 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha0 & (0x7 << 21)) >> 21];

        out[2 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataColor2 & (0x3 << 0)) >> 0];
        out[2 * outPitch + 0 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor2 & (0x3 << 0)) >> 0];
        out[2 * outPitch + 0 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor2 & (0x3 << 0)) >> 0];
        out[2 * outPitch + 0 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha1 & (0x7 << 0)) >> 0];

        out[2 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataColor2 & (0x3 << 2)) >> 2];
        out[2 * outPitch + 1 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor2 & (0x3 << 2)) >> 2];
        out[2 * outPitch + 1 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor2 & (0x3 << 2)) >> 2];
        out[2 * outPitch + 1 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha1 & (0x7 << 3)) >> 3];

        out[2 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataColor2 & (0x3 << 4)) >> 4];
        out[2 * outPitch + 2 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor2 & (0x3 << 4)) >> 4];
        out[2 * outPitch + 2 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor2 & (0x3 << 4)) >> 4];
        out[2 * outPitch + 2 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha1 & (0x7 << 6)) >> 6];

        out[2 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataColor2 & (0x3 << 6)) >> 6];
        out[2 * outPitch + 3 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor2 & (0x3 << 6)) >> 6];
        out[2 * outPitch + 3 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor2 & (0x3 << 6)) >> 6];
        out[2 * outPitch + 3 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha1 & (0x7 << 9)) >> 9];

        out[3 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataColor3 & (0x3 << 0)) >> 0];
        out[3 * outPitch + 0 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor3 & (0x3 << 0)) >> 0];
        out[3 * outPitch + 0 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor3 & (0x3 << 0)) >> 0];
        out[3 * outPitch + 0 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha1 & (0x7 << 12)) >> 12];

        out[3 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataColor3 & (0x3 << 2)) >> 2];
        out[3 * outPitch + 1 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor3 & (0x3 << 2)) >> 2];
        out[3 * outPitch + 1 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor3 & (0x3 << 2)) >> 2];
        out[3 * outPitch + 1 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha1 & (0x7 << 15)) >> 15];

        out[3 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataColor3 & (0x3 << 4)) >> 4];
        out[3 * outPitch + 2 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor3 & (0x3 << 4)) >> 4];
        out[3 * outPitch + 2 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor3 & (0x3 << 4)) >> 4];
        out[3 * outPitch + 2 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha1 & (0x7 << 18)) >> 18];

        out[3 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataColor3 & (0x3 << 6)) >> 6];
        out[3 * outPitch + 3 * outPixelSize + outOffsetG] = colorTableGreen[(dataColor3 & (0x3 << 6)) >> 6];
        out[3 * outPitch + 3 * outPixelSize + outOffsetB] = colorTableBlue[(dataColor3 & (0x3 << 6)) >> 6];
        out[3 * outPitch + 3 * outPixelSize + outOffsetA] = colorTableAlpha[(dataAlpha1 & (0x7 << 21)) >> 21];
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
        assert(width <= BC3_BLOCK_PIXELS);
        assert(height <= BC3_BLOCK_PIXELS);

        uint8_t colorTableRed[4];
        uint8_t colorTableGreen[4];
        uint8_t colorTableBlue[4];
        uint8_t colorTableAlpha[8];
        SetupColorTables(in, colorTableRed, colorTableGreen, colorTableBlue, colorTableAlpha);

        const uint32_t dataAlpha0 = in[2] | static_cast<uint32_t>(in[3] << 8u) | static_cast<uint32_t>(in[4] << 16u);
        const uint32_t dataAlpha1 = in[5] | static_cast<uint32_t>(in[6] << 8u) | static_cast<uint32_t>(in[7] << 16u);
        const uint8_t dataColor0 = in[BC3_ALPHA_CHANNEL_SIZE + 4];
        const uint8_t dataColor1 = in[BC3_ALPHA_CHANNEL_SIZE + 5];
        const uint8_t dataColor2 = in[BC3_ALPHA_CHANNEL_SIZE + 6];
        const uint8_t dataColor3 = in[BC3_ALPHA_CHANNEL_SIZE + 7];

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
        static_assert(std::extent_v<decltype(pixelsRed)> == BC3_BLOCK_PIXELS * BC3_BLOCK_PIXELS);

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
        static_assert(std::extent_v<decltype(pixelsGreen)> == BC3_BLOCK_PIXELS * BC3_BLOCK_PIXELS);

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
        static_assert(std::extent_v<decltype(pixelsBlue)> == BC3_BLOCK_PIXELS * BC3_BLOCK_PIXELS);

        const uint8_t pixelsAlpha[]{
            colorTableAlpha[(dataAlpha0 & (0x7 << 0)) >> 0],
            colorTableAlpha[(dataAlpha0 & (0x7 << 3)) >> 3],
            colorTableAlpha[(dataAlpha0 & (0x7 << 6)) >> 6],
            colorTableAlpha[(dataAlpha0 & (0x7 << 9)) >> 9],
            colorTableAlpha[(dataAlpha0 & (0x7 << 12)) >> 12],
            colorTableAlpha[(dataAlpha0 & (0x7 << 15)) >> 15],
            colorTableAlpha[(dataAlpha0 & (0x7 << 18)) >> 18],
            colorTableAlpha[(dataAlpha0 & (0x7 << 21)) >> 21],
            colorTableAlpha[(dataAlpha1 & (0x7 << 0)) >> 0],
            colorTableAlpha[(dataAlpha1 & (0x7 << 3)) >> 3],
            colorTableAlpha[(dataAlpha1 & (0x7 << 6)) >> 6],
            colorTableAlpha[(dataAlpha1 & (0x7 << 9)) >> 9],
            colorTableAlpha[(dataAlpha1 & (0x7 << 12)) >> 12],
            colorTableAlpha[(dataAlpha1 & (0x7 << 15)) >> 15],
            colorTableAlpha[(dataAlpha1 & (0x7 << 18)) >> 18],
            colorTableAlpha[(dataAlpha1 & (0x7 << 21)) >> 21],
        };
        static_assert(std::extent_v<decltype(pixelsAlpha)> == BC3_BLOCK_PIXELS * BC3_BLOCK_PIXELS);

        for (auto curHeight = 0u; curHeight < height; curHeight++)
        {
            for (auto curWidth = 0u; curWidth < width; curWidth++)
            {
                out[curHeight * outPitch + curWidth * outPixelSize + outOffsetR] = pixelsRed[curHeight * BC3_BLOCK_PIXELS + curWidth];
                out[curHeight * outPitch + curWidth * outPixelSize + outOffsetG] = pixelsGreen[curHeight * BC3_BLOCK_PIXELS + curWidth];
                out[curHeight * outPitch + curWidth * outPixelSize + outOffsetB] = pixelsBlue[curHeight * BC3_BLOCK_PIXELS + curWidth];
                out[curHeight * outPitch + curWidth * outPixelSize + outOffsetA] = pixelsAlpha[curHeight * BC3_BLOCK_PIXELS + curWidth];
            }
        }
    }
} // namespace

namespace image
{
    std::unique_ptr<Texture> DecompressorBc3::Decompress(const Texture& input, const ImageFormat* targetFormat)
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
                    const auto fullBlocksWidth = utils::AlignToPrevious(mipWidth, BC3_BLOCK_PIXELS);
                    const auto fullBlocksHeight = utils::AlignToPrevious(mipHeight, BC3_BLOCK_PIXELS);

                    for (auto curHeight = 0u; curHeight < fullBlocksHeight; curHeight += BC3_BLOCK_PIXELS)
                    {
                        for (auto curWidth = 0u; curWidth < fullBlocksWidth; curWidth += BC3_BLOCK_PIXELS)
                        {
                            DecompressBlock(bufferIn, bufferOut, mipPitch, outPixelSize, outOffsetR, outOffsetG, outOffsetB, outOffsetA);
                            bufferIn += BC3_BLOCK_SIZE;
                            bufferOut += outPixelSize * BC3_BLOCK_PIXELS;
                        }

                        if (fullBlocksWidth < mipWidth)
                        {
                            const auto edgeBlockWidth = mipWidth - fullBlocksWidth;
                            DecompressBlockEdge(
                                bufferIn, bufferOut, edgeBlockWidth, BC3_BLOCK_PIXELS, mipPitch, outPixelSize, outOffsetR, outOffsetG, outOffsetB, outOffsetA);
                            bufferIn += BC3_BLOCK_SIZE;
                            bufferOut += outPixelSize * edgeBlockWidth;
                        }

                        bufferOut += mipPitch * (BC3_BLOCK_PIXELS - 1);
                    }

                    if (fullBlocksHeight < mipHeight)
                    {
                        const auto edgeBlockHeight = mipHeight - fullBlocksHeight;
                        for (auto curWidth = 0u; curWidth < fullBlocksWidth; curWidth += BC3_BLOCK_PIXELS)
                        {
                            DecompressBlockEdge(
                                bufferIn, bufferOut, BC3_BLOCK_PIXELS, edgeBlockHeight, mipPitch, outPixelSize, outOffsetR, outOffsetG, outOffsetB, outOffsetA);
                            bufferIn += BC3_BLOCK_SIZE;
                            bufferOut += outPixelSize * BC3_BLOCK_PIXELS;
                        }

                        if (fullBlocksWidth < mipWidth)
                        {
                            const auto edgeBlockWidth = mipWidth - fullBlocksWidth;
                            DecompressBlockEdge(
                                bufferIn, bufferOut, edgeBlockWidth, edgeBlockHeight, mipPitch, outPixelSize, outOffsetR, outOffsetG, outOffsetB, outOffsetA);
                            bufferIn += BC3_BLOCK_SIZE;
                            bufferOut += outPixelSize * edgeBlockWidth;
                        }

                        bufferOut += mipPitch * (BC3_BLOCK_PIXELS - 1);
                    }
                }
            }
        }

        return result;
    }
} // namespace image
