#include "DecompressorBc5.h"

#include "Utils/Alignment.h"

#include <cassert>

// uint8_t pixelsRed[16]{
//     static_cast<uint8_t>(in[2] & 0x7),
//     static_cast<uint8_t>((in[2] & 0x38) >> 3),
//     static_cast<uint8_t>(((in[2] & 0xC0) >> 6) | ((in[3] & 0x1) << 2)),
//     static_cast<uint8_t>((in[3] & 0xE) >> 1),
//     static_cast<uint8_t>((in[3] & 0x70) >> 4),
//     static_cast<uint8_t>(((in[3] & 0x80) >> 7) | ((in[4] & 0x3) << 1)),
//     static_cast<uint8_t>((in[4] & 0x1C) >> 2),
//     static_cast<uint8_t>((in[4] & 0xE0) >> 5),
//     static_cast<uint8_t>(in[5] & 0x7),
//     static_cast<uint8_t>((in[5] & 0x38) >> 3),
//     static_cast<uint8_t>(((in[5] & 0xC0) >> 6) | ((in[6] & 0x1) << 2)),
//     static_cast<uint8_t>((in[6] & 0xE) >> 1),
//     static_cast<uint8_t>((in[6] & 0x70) >> 4),
//     static_cast<uint8_t>(((in[6] & 0x80) >> 7) | ((in[7] & 0x3) << 1)),
//     static_cast<uint8_t>((in[7] & 0x1C) >> 2),
//     static_cast<uint8_t>((in[7] & 0xE0) >> 5),
// };
// uint8_t pixelsGreen[16]{
//     static_cast<uint8_t>(in[BC5_CHANNEL_SIZE + 2] & 0x7),
//     static_cast<uint8_t>((in[BC5_CHANNEL_SIZE + 2] & 0x38) >> 3),
//     static_cast<uint8_t>(((in[BC5_CHANNEL_SIZE + 2] & 0xC0) >> 6) | ((in[BC5_CHANNEL_SIZE + 3] & 0x1) << 2)),
//     static_cast<uint8_t>((in[BC5_CHANNEL_SIZE + 3] & 0xE) >> 1),
//     static_cast<uint8_t>((in[BC5_CHANNEL_SIZE + 3] & 0x70) >> 4),
//     static_cast<uint8_t>(((in[BC5_CHANNEL_SIZE + 3] & 0x80) >> 7) | ((in[BC5_CHANNEL_SIZE + 4] & 0x3) << 1)),
//     static_cast<uint8_t>((in[BC5_CHANNEL_SIZE + 4] & 0x1C) >> 2),
//     static_cast<uint8_t>((in[BC5_CHANNEL_SIZE + 4] & 0xE0) >> 5),
//     static_cast<uint8_t>(in[BC5_CHANNEL_SIZE + 5] & 0x7),
//     static_cast<uint8_t>((in[BC5_CHANNEL_SIZE + 5] & 0x38) >> 3),
//     static_cast<uint8_t>(((in[BC5_CHANNEL_SIZE + 5] & 0xC0) >> 6) | ((in[BC5_CHANNEL_SIZE + 6] & 0x1) << 2)),
//     static_cast<uint8_t>((in[BC5_CHANNEL_SIZE + 6] & 0xE) >> 1),
//     static_cast<uint8_t>((in[BC5_CHANNEL_SIZE + 6] & 0x70) >> 4),
//     static_cast<uint8_t>(((in[BC5_CHANNEL_SIZE + 6] & 0x80) >> 7) | ((in[BC5_CHANNEL_SIZE + 7] & 0x3) << 1)),
//     static_cast<uint8_t>((in[BC5_CHANNEL_SIZE + 7] & 0x1C) >> 2),
//     static_cast<uint8_t>((in[BC5_CHANNEL_SIZE + 7] & 0xE0) >> 5),
// };
// in[2] & 0x7
// (in[2] & 0x38) >> 3
// ((in[2] & 0xC0) >> 6) | ((in[3] & 0x1) << 2)
// (in[3] & 0xE) >> 1
// (in[3] & 0x70) >> 4
// ((in[3] & 0x80) >> 7) | ((in[4] & 0x3) << 1)
// (in[4] & 0x1C) >> 2
// (in[4] & 0xE0) >> 5

namespace
{
    constexpr auto BC5_BLOCK_PIXELS = 4u;
    constexpr auto BC5_CHANNEL_SIZE = 8u;
    constexpr auto BC5_BLOCK_SIZE = BC5_CHANNEL_SIZE * 2u;
    constexpr auto OUT_PIXEL_SIZE = 3u;
    constexpr auto OUT_OFFSET_R = 0u;
    constexpr auto OUT_OFFSET_G = 1u;

    void SetupColorTables(const uint8_t* in, uint8_t (&colorTableRed)[8], uint8_t (&colorTableGreen)[8])
    {
#define INTERPOLATE_BC5(colorTable, val0, val1, divide)                                                                                                        \
    static_cast<uint8_t>(static_cast<float>(static_cast<uint16_t>(val0) * static_cast<uint16_t>((colorTable)[0])                                               \
                                            + static_cast<uint16_t>(val1) * static_cast<uint16_t>((colorTable)[1]))                                            \
                         / (divide))

        colorTableRed[0] = in[0];
        colorTableRed[1] = in[1];

        if (colorTableRed[0] > colorTableRed[1])
        {
            // 6 interpolated color values
            colorTableRed[2] = INTERPOLATE_BC5(colorTableRed, 6, 1, 7.0f);
            colorTableRed[3] = INTERPOLATE_BC5(colorTableRed, 5, 2, 7.0f);
            colorTableRed[4] = INTERPOLATE_BC5(colorTableRed, 4, 3, 7.0f);
            colorTableRed[5] = INTERPOLATE_BC5(colorTableRed, 3, 4, 7.0f);
            colorTableRed[6] = INTERPOLATE_BC5(colorTableRed, 2, 5, 7.0f);
            colorTableRed[7] = INTERPOLATE_BC5(colorTableRed, 1, 6, 7.0f);
        }
        else
        {
            // 4 interpolated color values
            colorTableRed[2] = INTERPOLATE_BC5(colorTableRed, 4, 1, 5.0f);
            colorTableRed[3] = INTERPOLATE_BC5(colorTableRed, 3, 2, 5.0f);
            colorTableRed[4] = INTERPOLATE_BC5(colorTableRed, 2, 3, 5.0f);
            colorTableRed[5] = INTERPOLATE_BC5(colorTableRed, 1, 4, 5.0f);
            colorTableRed[6] = std::numeric_limits<uint8_t>::min();
            colorTableRed[7] = std::numeric_limits<uint8_t>::max();
        }

        colorTableGreen[0] = in[BC5_CHANNEL_SIZE + 0];
        colorTableGreen[1] = in[BC5_CHANNEL_SIZE + 1];

        if (colorTableGreen[0] > colorTableGreen[1])
        {
            // 6 interpolated color values
            colorTableGreen[2] = INTERPOLATE_BC5(colorTableGreen, 6, 1, 7.0f);
            colorTableGreen[3] = INTERPOLATE_BC5(colorTableGreen, 5, 2, 7.0f);
            colorTableGreen[4] = INTERPOLATE_BC5(colorTableGreen, 4, 3, 7.0f);
            colorTableGreen[5] = INTERPOLATE_BC5(colorTableGreen, 3, 4, 7.0f);
            colorTableGreen[6] = INTERPOLATE_BC5(colorTableGreen, 2, 5, 7.0f);
            colorTableGreen[7] = INTERPOLATE_BC5(colorTableGreen, 1, 6, 7.0f);
        }
        else
        {
            // 4 interpolated color values
            colorTableGreen[2] = INTERPOLATE_BC5(colorTableGreen, 4, 1, 5.0f);
            colorTableGreen[3] = INTERPOLATE_BC5(colorTableGreen, 3, 2, 5.0f);
            colorTableGreen[4] = INTERPOLATE_BC5(colorTableGreen, 2, 3, 5.0f);
            colorTableGreen[5] = INTERPOLATE_BC5(colorTableGreen, 1, 4, 5.0f);
            colorTableGreen[6] = std::numeric_limits<uint8_t>::min();
            colorTableGreen[7] = std::numeric_limits<uint8_t>::max();
        }

#undef INTERPOLATE_BC5
    }

    void DecompressBlock(const uint8_t* in, uint8_t* out, const unsigned outPitch)
    {
        uint8_t colorTableRed[8];
        uint8_t colorTableGreen[8];
        SetupColorTables(in, colorTableRed, colorTableGreen);
        const uint32_t dataRed0 = in[2] | static_cast<uint32_t>(in[3] << 8u) | static_cast<uint32_t>(in[4] << 16u);
        const uint32_t dataRed1 = in[5] | static_cast<uint32_t>(in[6] << 8u) | static_cast<uint32_t>(in[7] << 16u);
        const uint32_t dataGreen0 =
            in[BC5_CHANNEL_SIZE + 2] | static_cast<uint32_t>(in[BC5_CHANNEL_SIZE + 3] << 8) | static_cast<uint32_t>(in[BC5_CHANNEL_SIZE + 4] << 16);
        const uint32_t dataGreen1 =
            in[BC5_CHANNEL_SIZE + 5] | static_cast<uint32_t>(in[BC5_CHANNEL_SIZE + 6] << 8) | static_cast<uint32_t>(in[BC5_CHANNEL_SIZE + 7] << 16);

        out[0 * outPitch + 0 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed0 & (0x7 << 21) >> 21];
        out[0 * outPitch + 0 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen0 & (0x7 << 21) >> 21];

        out[0 * outPitch + 1 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed0 & (0x7 << 18) >> 18];
        out[0 * outPitch + 1 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen0 & (0x7 << 18) >> 18];

        out[0 * outPitch + 2 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed0 & (0x7 << 15) >> 15];
        out[0 * outPitch + 2 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen0 & (0x7 << 15) >> 15];

        out[0 * outPitch + 3 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed0 & (0x7 << 12) >> 12];
        out[0 * outPitch + 3 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen0 & (0x7 << 12) >> 12];

        out[1 * outPitch + 0 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed0 & (0x7 << 9) >> 9];
        out[1 * outPitch + 0 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen0 & (0x7 << 9) >> 9];

        out[1 * outPitch + 1 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed0 & (0x7 << 6) >> 6];
        out[1 * outPitch + 1 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen0 & (0x7 << 6) >> 6];

        out[1 * outPitch + 2 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed0 & (0x7 << 3) >> 3];
        out[1 * outPitch + 2 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen0 & (0x7 << 3) >> 3];

        out[1 * outPitch + 3 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed0 & (0x7 << 0) >> 0];
        out[1 * outPitch + 3 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen0 & (0x7 << 0) >> 0];

        out[2 * outPitch + 0 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed1 & (0x7 << 21) >> 21];
        out[2 * outPitch + 0 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen1 & (0x7 << 21) >> 21];

        out[2 * outPitch + 1 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed1 & (0x7 << 18) >> 18];
        out[2 * outPitch + 1 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen1 & (0x7 << 18) >> 18];

        out[2 * outPitch + 2 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed1 & (0x7 << 15) >> 15];
        out[2 * outPitch + 2 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen1 & (0x7 << 15) >> 15];

        out[2 * outPitch + 3 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed1 & (0x7 << 12) >> 12];
        out[2 * outPitch + 3 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen1 & (0x7 << 12) >> 12];

        out[3 * outPitch + 0 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed1 & (0x7 << 9) >> 9];
        out[3 * outPitch + 0 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen1 & (0x7 << 9) >> 9];

        out[3 * outPitch + 1 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed1 & (0x7 << 6) >> 6];
        out[3 * outPitch + 1 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen1 & (0x7 << 6) >> 6];

        out[3 * outPitch + 2 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed1 & (0x7 << 3) >> 3];
        out[3 * outPitch + 2 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen1 & (0x7 << 3) >> 3];

        out[3 * outPitch + 3 * OUT_PIXEL_SIZE + OUT_OFFSET_R] = colorTableRed[dataRed1 & (0x7 << 0) >> 0];
        out[3 * outPitch + 3 * OUT_PIXEL_SIZE + OUT_OFFSET_G] = colorTableGreen[dataGreen1 & (0x7 << 0) >> 0];
    }

    void DecompressBlockEdge(const uint8_t* in, uint8_t* out, const unsigned outPitch, const unsigned width, const unsigned height)
    {
        assert(width <= BC5_BLOCK_PIXELS);
        assert(height <= BC5_BLOCK_PIXELS);

        uint8_t colorTableRed[8];
        uint8_t colorTableGreen[8];
        SetupColorTables(in, colorTableRed, colorTableGreen);

        const uint32_t dataRed0 = in[2] | static_cast<uint32_t>(in[3] << 8u) | static_cast<uint32_t>(in[4] << 16u);
        const uint32_t dataRed1 = in[5] | static_cast<uint32_t>(in[6] << 8u) | static_cast<uint32_t>(in[7] << 16u);
        const uint32_t dataGreen0 =
            in[BC5_CHANNEL_SIZE + 2] | static_cast<uint32_t>(in[BC5_CHANNEL_SIZE + 3] << 8) | static_cast<uint32_t>(in[BC5_CHANNEL_SIZE + 4] << 16);
        const uint32_t dataGreen1 =
            in[BC5_CHANNEL_SIZE + 5] | static_cast<uint32_t>(in[BC5_CHANNEL_SIZE + 6] << 8) | static_cast<uint32_t>(in[BC5_CHANNEL_SIZE + 7] << 16);

        const uint8_t pixelsRed[]{
            colorTableRed[dataRed0 & (0x7 << 21) >> 21],
            colorTableRed[dataRed0 & (0x7 << 18) >> 18],
            colorTableRed[dataRed0 & (0x7 << 15) >> 15],
            colorTableRed[dataRed0 & (0x7 << 12) >> 12],
            colorTableRed[dataRed0 & (0x7 << 9) >> 9],
            colorTableRed[dataRed0 & (0x7 << 6) >> 6],
            colorTableRed[dataRed0 & (0x7 << 3) >> 3],
            colorTableRed[dataRed0 & (0x7 << 0) >> 0],
            colorTableRed[dataRed1 & (0x7 << 21) >> 21],
            colorTableRed[dataRed1 & (0x7 << 18) >> 18],
            colorTableRed[dataRed1 & (0x7 << 15) >> 15],
            colorTableRed[dataRed1 & (0x7 << 12) >> 12],
            colorTableRed[dataRed1 & (0x7 << 9) >> 9],
            colorTableRed[dataRed1 & (0x7 << 6) >> 6],
            colorTableRed[dataRed1 & (0x7 << 3) >> 3],
            colorTableRed[dataRed1 & (0x7 << 0) >> 0],
        };
        static_assert(std::extent_v<decltype(pixelsRed)> == BC5_BLOCK_PIXELS * BC5_BLOCK_PIXELS);

        const uint8_t pixelsGreen[]{
            colorTableGreen[dataGreen0 & (0x7 << 21) >> 21],
            colorTableGreen[dataGreen0 & (0x7 << 18) >> 18],
            colorTableGreen[dataGreen0 & (0x7 << 15) >> 15],
            colorTableGreen[dataGreen0 & (0x7 << 12) >> 12],
            colorTableGreen[dataGreen0 & (0x7 << 9) >> 9],
            colorTableGreen[dataGreen0 & (0x7 << 6) >> 6],
            colorTableGreen[dataGreen0 & (0x7 << 3) >> 3],
            colorTableGreen[dataGreen0 & (0x7 << 0) >> 0],
            colorTableGreen[dataGreen1 & (0x7 << 21) >> 21],
            colorTableGreen[dataGreen1 & (0x7 << 18) >> 18],
            colorTableGreen[dataGreen1 & (0x7 << 15) >> 15],
            colorTableGreen[dataGreen1 & (0x7 << 12) >> 12],
            colorTableGreen[dataGreen1 & (0x7 << 9) >> 9],
            colorTableGreen[dataGreen1 & (0x7 << 6) >> 6],
            colorTableGreen[dataGreen1 & (0x7 << 3) >> 3],
            colorTableGreen[dataGreen1 & (0x7 << 0) >> 0],
        };
        static_assert(std::extent_v<decltype(pixelsGreen)> == BC5_BLOCK_PIXELS * BC5_BLOCK_PIXELS);

        for (auto curHeight = 0u; curHeight < height; curHeight++)
        {
            for (auto curWidth = 0u; curWidth < width; curWidth++)
            {
                out[curHeight * outPitch + curWidth * OUT_PIXEL_SIZE + OUT_OFFSET_R] = pixelsRed[curHeight * BC5_BLOCK_PIXELS + curWidth];
                out[curHeight * outPitch + curWidth * OUT_PIXEL_SIZE + OUT_OFFSET_G] = pixelsGreen[curHeight * BC5_BLOCK_PIXELS + curWidth];
            }
        }
    }
} // namespace

namespace image
{
    std::unique_ptr<Texture> DecompressorBc5::Decompress(const Texture& input)
    {
        const auto width = input.GetWidth();
        const auto height = input.GetHeight();
        const auto depth = input.GetDepth();

        auto result = Texture::CreateForType(input.GetTextureType(), &ImageFormat::FORMAT_R8_G8_B8, width, height, depth, input.HasMipMaps());
        result->Allocate();

        const auto faceCount = result->GetFaceCount();
        const auto mipCount = result->GetMipMapCount();
        assert(mipCount == input.GetMipMapCount());

        for (auto mipLevel = 0; mipLevel < mipCount; mipLevel++)
        {
            const unsigned mipWidth = width >> mipLevel;
            const unsigned mipHeight = height >> mipLevel;
            const unsigned mipDepth = depth >> mipLevel;
            const unsigned mipPitch = OUT_PIXEL_SIZE * mipWidth;
            assert(mipPitch == result->GetFormat()->GetPitch(mipLevel, width));

            for (auto face = 0; face < faceCount; face++)
            {
                const auto* bufferIn = input.GetBufferForMipLevel(mipLevel, face);
                auto* bufferOut = result->GetBufferForMipLevel(mipLevel, face);

                for (auto curDepth = 0u; curDepth < mipDepth; curDepth++)
                {
                    const auto fullBlocksWidth = utils::AlignToPrevious(mipWidth, BC5_BLOCK_PIXELS);
                    const auto fullBlocksHeight = utils::AlignToPrevious(mipHeight, BC5_BLOCK_PIXELS);

                    for (auto curHeight = 0u; curHeight < fullBlocksHeight; curHeight += BC5_BLOCK_PIXELS)
                    {
                        for (auto curWidth = 0u; curWidth < fullBlocksWidth; curWidth += BC5_BLOCK_PIXELS)
                        {
                            DecompressBlock(bufferIn, bufferOut, mipPitch);
                            bufferIn += BC5_BLOCK_SIZE;
                            bufferOut += OUT_PIXEL_SIZE * BC5_BLOCK_PIXELS;
                        }

                        if (fullBlocksWidth < mipWidth)
                        {
                            const auto edgeBlockWidth = mipWidth - fullBlocksWidth;
                            DecompressBlockEdge(bufferIn, bufferOut, mipPitch, edgeBlockWidth, BC5_BLOCK_PIXELS);
                            bufferIn += BC5_BLOCK_SIZE;
                            bufferOut += OUT_PIXEL_SIZE * edgeBlockWidth;
                        }
                    }

                    if (fullBlocksHeight < mipHeight)
                    {
                        const auto edgeBlockHeight = mipHeight - fullBlocksHeight;
                        for (auto curWidth = 0u; curWidth < fullBlocksWidth; curWidth += BC5_BLOCK_PIXELS)
                        {
                            DecompressBlockEdge(bufferIn, bufferOut, mipPitch, BC5_BLOCK_PIXELS, edgeBlockHeight);
                            bufferIn += BC5_BLOCK_SIZE;
                            bufferOut += OUT_PIXEL_SIZE * BC5_BLOCK_PIXELS;
                        }

                        if (fullBlocksWidth < mipWidth)
                        {
                            const auto edgeBlockWidth = mipWidth - fullBlocksWidth;
                            DecompressBlockEdge(bufferIn, bufferOut, mipPitch, edgeBlockWidth, edgeBlockHeight);
                            bufferIn += BC5_BLOCK_SIZE;
                            bufferOut += OUT_PIXEL_SIZE * edgeBlockWidth;
                        }
                    }
                }
            }
        }

        return result;
    }
} // namespace image
