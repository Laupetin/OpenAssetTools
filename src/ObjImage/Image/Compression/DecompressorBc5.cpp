#include "DecompressorBc5.h"

#include "Utils/Alignment.h"

#include <cassert>
#include <limits>

namespace
{
    constexpr auto BC5_BLOCK_PIXELS = 4u;
    constexpr auto BC5_CHANNEL_SIZE = 8u;
    constexpr auto BC5_BLOCK_SIZE = BC5_CHANNEL_SIZE * 2u;

    void SetupColorTables(const uint8_t* in, uint8_t (&colorTableRed)[8], uint8_t (&colorTableGreen)[8])
    {
#define INTERPOLATE_BC5(colorTable, val0, val1)                                                                                                                \
    static_cast<uint8_t>(                                                                                                                                      \
        (static_cast<uint16_t>(val0) * static_cast<uint16_t>((colorTable)[0]) + static_cast<uint16_t>(val1) * static_cast<uint16_t>((colorTable)[1]))          \
        / ((val0) + (val1)))

        colorTableRed[0] = in[0];
        colorTableRed[1] = in[1];

        if (colorTableRed[0] > colorTableRed[1])
        {
            // 6 interpolated color values
            colorTableRed[2] = INTERPOLATE_BC5(colorTableRed, 6, 1);
            colorTableRed[3] = INTERPOLATE_BC5(colorTableRed, 5, 2);
            colorTableRed[4] = INTERPOLATE_BC5(colorTableRed, 4, 3);
            colorTableRed[5] = INTERPOLATE_BC5(colorTableRed, 3, 4);
            colorTableRed[6] = INTERPOLATE_BC5(colorTableRed, 2, 5);
            colorTableRed[7] = INTERPOLATE_BC5(colorTableRed, 1, 6);
        }
        else
        {
            // 4 interpolated color values
            colorTableRed[2] = INTERPOLATE_BC5(colorTableRed, 4, 1);
            colorTableRed[3] = INTERPOLATE_BC5(colorTableRed, 3, 2);
            colorTableRed[4] = INTERPOLATE_BC5(colorTableRed, 2, 3);
            colorTableRed[5] = INTERPOLATE_BC5(colorTableRed, 1, 4);
            colorTableRed[6] = std::numeric_limits<uint8_t>::min();
            colorTableRed[7] = std::numeric_limits<uint8_t>::max();
        }

        colorTableGreen[0] = in[BC5_CHANNEL_SIZE + 0];
        colorTableGreen[1] = in[BC5_CHANNEL_SIZE + 1];

        if (colorTableGreen[0] > colorTableGreen[1])
        {
            // 6 interpolated color values
            colorTableGreen[2] = INTERPOLATE_BC5(colorTableGreen, 6, 1);
            colorTableGreen[3] = INTERPOLATE_BC5(colorTableGreen, 5, 2);
            colorTableGreen[4] = INTERPOLATE_BC5(colorTableGreen, 4, 3);
            colorTableGreen[5] = INTERPOLATE_BC5(colorTableGreen, 3, 4);
            colorTableGreen[6] = INTERPOLATE_BC5(colorTableGreen, 2, 5);
            colorTableGreen[7] = INTERPOLATE_BC5(colorTableGreen, 1, 6);
        }
        else
        {
            // 4 interpolated color values
            colorTableGreen[2] = INTERPOLATE_BC5(colorTableGreen, 4, 1);
            colorTableGreen[3] = INTERPOLATE_BC5(colorTableGreen, 3, 2);
            colorTableGreen[4] = INTERPOLATE_BC5(colorTableGreen, 2, 3);
            colorTableGreen[5] = INTERPOLATE_BC5(colorTableGreen, 1, 4);
            colorTableGreen[6] = std::numeric_limits<uint8_t>::min();
            colorTableGreen[7] = std::numeric_limits<uint8_t>::max();
        }

#undef INTERPOLATE_BC5
    }

    void DecompressBlock(
        const uint8_t* in, uint8_t* out, const unsigned outPitch, const unsigned outPixelSize, const unsigned outOffsetR, const unsigned outOffsetG)
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

        out[0 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 0)) >> 0];
        out[0 * outPitch + 0 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen0 & (0x7 << 0)) >> 0];

        out[0 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 3)) >> 3];
        out[0 * outPitch + 1 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen0 & (0x7 << 3)) >> 3];

        out[0 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 6)) >> 6];
        out[0 * outPitch + 2 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen0 & (0x7 << 6)) >> 6];

        out[0 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 9)) >> 9];
        out[0 * outPitch + 3 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen0 & (0x7 << 9)) >> 9];

        out[1 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 12)) >> 12];
        out[1 * outPitch + 0 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen0 & (0x7 << 12)) >> 12];

        out[1 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 15)) >> 15];
        out[1 * outPitch + 1 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen0 & (0x7 << 15)) >> 15];

        out[1 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 18)) >> 18];
        out[1 * outPitch + 2 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen0 & (0x7 << 18)) >> 18];

        out[1 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 21)) >> 21];
        out[1 * outPitch + 3 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen0 & (0x7 << 21)) >> 21];

        out[2 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 0)) >> 0];
        out[2 * outPitch + 0 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen1 & (0x7 << 0)) >> 0];

        out[2 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 3)) >> 3];
        out[2 * outPitch + 1 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen1 & (0x7 << 3)) >> 3];

        out[2 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 6)) >> 6];
        out[2 * outPitch + 2 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen1 & (0x7 << 6)) >> 6];

        out[2 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 9)) >> 9];
        out[2 * outPitch + 3 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen1 & (0x7 << 9)) >> 9];

        out[3 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 12)) >> 12];
        out[3 * outPitch + 0 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen1 & (0x7 << 12)) >> 12];

        out[3 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 15)) >> 15];
        out[3 * outPitch + 1 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen1 & (0x7 << 15)) >> 15];

        out[3 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 18)) >> 18];
        out[3 * outPitch + 2 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen1 & (0x7 << 18)) >> 18];

        out[3 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 21)) >> 21];
        out[3 * outPitch + 3 * outPixelSize + outOffsetG] = colorTableGreen[(dataGreen1 & (0x7 << 21)) >> 21];
    }

    void DecompressBlockEdge(const uint8_t* in,
                             uint8_t* out,
                             const unsigned width,
                             const unsigned height,
                             const unsigned outPitch,
                             const unsigned outPixelSize,
                             const unsigned outOffsetR,
                             const unsigned outOffsetG)
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
            colorTableRed[(dataRed0 & (0x7 << 0)) >> 0],
            colorTableRed[(dataRed0 & (0x7 << 3)) >> 3],
            colorTableRed[(dataRed0 & (0x7 << 6)) >> 6],
            colorTableRed[(dataRed0 & (0x7 << 9)) >> 9],
            colorTableRed[(dataRed0 & (0x7 << 12)) >> 12],
            colorTableRed[(dataRed0 & (0x7 << 15)) >> 15],
            colorTableRed[(dataRed0 & (0x7 << 18)) >> 18],
            colorTableRed[(dataRed0 & (0x7 << 21)) >> 21],
            colorTableRed[(dataRed1 & (0x7 << 0)) >> 0],
            colorTableRed[(dataRed1 & (0x7 << 3)) >> 3],
            colorTableRed[(dataRed1 & (0x7 << 6)) >> 6],
            colorTableRed[(dataRed1 & (0x7 << 9)) >> 9],
            colorTableRed[(dataRed1 & (0x7 << 12)) >> 12],
            colorTableRed[(dataRed1 & (0x7 << 15)) >> 15],
            colorTableRed[(dataRed1 & (0x7 << 18)) >> 18],
            colorTableRed[(dataRed1 & (0x7 << 21)) >> 21],
        };
        static_assert(std::extent_v<decltype(pixelsRed)> == BC5_BLOCK_PIXELS * BC5_BLOCK_PIXELS);

        const uint8_t pixelsGreen[]{
            colorTableGreen[(dataGreen0 & (0x7 << 0)) >> 0],
            colorTableGreen[(dataGreen0 & (0x7 << 3)) >> 3],
            colorTableGreen[(dataGreen0 & (0x7 << 6)) >> 6],
            colorTableGreen[(dataGreen0 & (0x7 << 9)) >> 9],
            colorTableGreen[(dataGreen0 & (0x7 << 12)) >> 12],
            colorTableGreen[(dataGreen0 & (0x7 << 15)) >> 15],
            colorTableGreen[(dataGreen0 & (0x7 << 18)) >> 18],
            colorTableGreen[(dataGreen0 & (0x7 << 21)) >> 21],
            colorTableGreen[(dataGreen1 & (0x7 << 0)) >> 0],
            colorTableGreen[(dataGreen1 & (0x7 << 3)) >> 3],
            colorTableGreen[(dataGreen1 & (0x7 << 6)) >> 6],
            colorTableGreen[(dataGreen1 & (0x7 << 9)) >> 9],
            colorTableGreen[(dataGreen1 & (0x7 << 12)) >> 12],
            colorTableGreen[(dataGreen1 & (0x7 << 15)) >> 15],
            colorTableGreen[(dataGreen1 & (0x7 << 18)) >> 18],
            colorTableGreen[(dataGreen1 & (0x7 << 21)) >> 21],
        };
        static_assert(std::extent_v<decltype(pixelsGreen)> == BC5_BLOCK_PIXELS * BC5_BLOCK_PIXELS);

        for (auto curHeight = 0u; curHeight < height; curHeight++)
        {
            for (auto curWidth = 0u; curWidth < width; curWidth++)
            {
                out[curHeight * outPitch + curWidth * outPixelSize + outOffsetR] = pixelsRed[curHeight * BC5_BLOCK_PIXELS + curWidth];
                out[curHeight * outPitch + curWidth * outPixelSize + outOffsetG] = pixelsGreen[curHeight * BC5_BLOCK_PIXELS + curWidth];
            }
        }
    }
} // namespace

namespace image
{
    std::unique_ptr<Texture> DecompressorBc5::Decompress(const Texture& input, const ImageFormat* targetFormat)
    {
        assert(targetFormat->GetType() == ImageFormatType::UNSIGNED);
        if (targetFormat->GetType() != ImageFormatType::UNSIGNED)
            return nullptr;

        const auto* unsignedTargetFormat = dynamic_cast<const ImageFormatUnsigned*>(targetFormat);

        const auto outPixelSize = unsignedTargetFormat->m_bits_per_pixel / 8u;

        // Only support formats with byte aligned channels
        const auto redByteAligned = unsignedTargetFormat->m_r_size == 8 && (unsignedTargetFormat->m_r_offset % 8) == 0;
        const auto greenByteAligned = unsignedTargetFormat->m_g_size == 8 && (unsignedTargetFormat->m_g_offset % 8) == 0;
        assert(redByteAligned);
        assert(greenByteAligned);
        if (!redByteAligned || !greenByteAligned)
            return nullptr;

        const auto outOffsetR = unsignedTargetFormat->m_r_offset / 8;
        const auto outOffsetG = unsignedTargetFormat->m_g_offset / 8;

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
                    const auto fullBlocksWidth = utils::AlignToPrevious(mipWidth, BC5_BLOCK_PIXELS);
                    const auto fullBlocksHeight = utils::AlignToPrevious(mipHeight, BC5_BLOCK_PIXELS);

                    for (auto curHeight = 0u; curHeight < fullBlocksHeight; curHeight += BC5_BLOCK_PIXELS)
                    {
                        for (auto curWidth = 0u; curWidth < fullBlocksWidth; curWidth += BC5_BLOCK_PIXELS)
                        {
                            DecompressBlock(bufferIn, bufferOut, mipPitch, outPixelSize, outOffsetR, outOffsetG);
                            bufferIn += BC5_BLOCK_SIZE;
                            bufferOut += outPixelSize * BC5_BLOCK_PIXELS;
                        }

                        if (fullBlocksWidth < mipWidth)
                        {
                            const auto edgeBlockWidth = mipWidth - fullBlocksWidth;
                            DecompressBlockEdge(bufferIn, bufferOut, edgeBlockWidth, BC5_BLOCK_PIXELS, mipPitch, outPixelSize, outOffsetR, outOffsetG);
                            bufferIn += BC5_BLOCK_SIZE;
                            bufferOut += outPixelSize * edgeBlockWidth;
                        }

                        bufferOut += mipPitch * (BC5_BLOCK_PIXELS - 1);
                    }

                    if (fullBlocksHeight < mipHeight)
                    {
                        const auto edgeBlockHeight = mipHeight - fullBlocksHeight;
                        for (auto curWidth = 0u; curWidth < fullBlocksWidth; curWidth += BC5_BLOCK_PIXELS)
                        {
                            DecompressBlockEdge(bufferIn, bufferOut, BC5_BLOCK_PIXELS, edgeBlockHeight, mipPitch, outPixelSize, outOffsetR, outOffsetG);
                            bufferIn += BC5_BLOCK_SIZE;
                            bufferOut += outPixelSize * BC5_BLOCK_PIXELS;
                        }

                        if (fullBlocksWidth < mipWidth)
                        {
                            const auto edgeBlockWidth = mipWidth - fullBlocksWidth;
                            DecompressBlockEdge(bufferIn, bufferOut, edgeBlockWidth, edgeBlockHeight, mipPitch, outPixelSize, outOffsetR, outOffsetG);
                            bufferIn += BC5_BLOCK_SIZE;
                            bufferOut += outPixelSize * edgeBlockWidth;
                        }

                        bufferOut += mipPitch * (BC5_BLOCK_PIXELS - 1);
                    }
                }
            }
        }

        return result;
    }
} // namespace image
