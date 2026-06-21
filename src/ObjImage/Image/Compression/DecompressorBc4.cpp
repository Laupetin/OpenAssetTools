#include "DecompressorBc4.h"

#include "Utils/Alignment.h"

#include <cassert>
#include <limits>

namespace
{
    constexpr auto BC4_BLOCK_PIXELS = 4u;
    constexpr auto BC4_CHANNEL_SIZE = 8u;
    constexpr auto BC4_BLOCK_SIZE = BC4_CHANNEL_SIZE;

    void SetupColorTable(const uint8_t* in, uint8_t (&colorTableRed)[8])
    {
#define INTERPOLATE_BC4(colorTable, val0, val1)                                                                                                                \
    static_cast<uint8_t>(                                                                                                                                      \
        (static_cast<uint16_t>(val0) * static_cast<uint16_t>((colorTable)[0]) + static_cast<uint16_t>(val1) * static_cast<uint16_t>((colorTable)[1]))          \
        / ((val0) + (val1)))

        colorTableRed[0] = in[0];
        colorTableRed[1] = in[1];

        if (colorTableRed[0] > colorTableRed[1])
        {
            // 6 interpolated color values
            colorTableRed[2] = INTERPOLATE_BC4(colorTableRed, 6, 1);
            colorTableRed[3] = INTERPOLATE_BC4(colorTableRed, 5, 2);
            colorTableRed[4] = INTERPOLATE_BC4(colorTableRed, 4, 3);
            colorTableRed[5] = INTERPOLATE_BC4(colorTableRed, 3, 4);
            colorTableRed[6] = INTERPOLATE_BC4(colorTableRed, 2, 5);
            colorTableRed[7] = INTERPOLATE_BC4(colorTableRed, 1, 6);
        }
        else
        {
            // 4 interpolated color values
            colorTableRed[2] = INTERPOLATE_BC4(colorTableRed, 4, 1);
            colorTableRed[3] = INTERPOLATE_BC4(colorTableRed, 3, 2);
            colorTableRed[4] = INTERPOLATE_BC4(colorTableRed, 2, 3);
            colorTableRed[5] = INTERPOLATE_BC4(colorTableRed, 1, 4);
            colorTableRed[6] = std::numeric_limits<uint8_t>::min();
            colorTableRed[7] = std::numeric_limits<uint8_t>::max();
        }

#undef INTERPOLATE_BC4
    }

    void DecompressBlock(const uint8_t* in, uint8_t* out, const unsigned outPitch, const unsigned outPixelSize, const unsigned outOffsetR)
    {
        uint8_t colorTableRed[8];
        SetupColorTable(in, colorTableRed);
        const uint32_t dataRed0 = in[2] | static_cast<uint32_t>(in[3] << 8u) | static_cast<uint32_t>(in[4] << 16u);
        const uint32_t dataRed1 = in[5] | static_cast<uint32_t>(in[6] << 8u) | static_cast<uint32_t>(in[7] << 16u);

        out[0 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 0)) >> 0];
        out[0 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 3)) >> 3];
        out[0 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 6)) >> 6];
        out[0 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 9)) >> 9];
        out[1 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 12)) >> 12];
        out[1 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 15)) >> 15];
        out[1 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 18)) >> 18];
        out[1 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataRed0 & (0x7 << 21)) >> 21];
        out[2 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 0)) >> 0];
        out[2 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 3)) >> 3];
        out[2 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 6)) >> 6];
        out[2 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 9)) >> 9];
        out[3 * outPitch + 0 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 12)) >> 12];
        out[3 * outPitch + 1 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 15)) >> 15];
        out[3 * outPitch + 2 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 18)) >> 18];
        out[3 * outPitch + 3 * outPixelSize + outOffsetR] = colorTableRed[(dataRed1 & (0x7 << 21)) >> 21];
    }

    void DecompressBlockEdge(const uint8_t* in,
                             uint8_t* out,
                             const unsigned width,
                             const unsigned height,
                             const unsigned outPitch,
                             const unsigned outPixelSize,
                             const unsigned outOffsetR)
    {
        assert(width <= BC4_BLOCK_PIXELS);
        assert(height <= BC4_BLOCK_PIXELS);

        uint8_t colorTableRed[8];
        SetupColorTable(in, colorTableRed);

        const uint32_t dataRed0 = in[2] | static_cast<uint32_t>(in[3] << 8u) | static_cast<uint32_t>(in[4] << 16u);
        const uint32_t dataRed1 = in[5] | static_cast<uint32_t>(in[6] << 8u) | static_cast<uint32_t>(in[7] << 16u);

        const uint8_t pixelsRed[]{
            colorTableRed[(dataRed0 & (0x7 << 0)) >> 21],
            colorTableRed[(dataRed0 & (0x7 << 3)) >> 18],
            colorTableRed[(dataRed0 & (0x7 << 6)) >> 15],
            colorTableRed[(dataRed0 & (0x7 << 9)) >> 12],
            colorTableRed[(dataRed0 & (0x7 << 12)) >> 9],
            colorTableRed[(dataRed0 & (0x7 << 15)) >> 6],
            colorTableRed[(dataRed0 & (0x7 << 18)) >> 3],
            colorTableRed[(dataRed0 & (0x7 << 21)) >> 0],
            colorTableRed[(dataRed1 & (0x7 << 0)) >> 21],
            colorTableRed[(dataRed1 & (0x7 << 3)) >> 18],
            colorTableRed[(dataRed1 & (0x7 << 6)) >> 15],
            colorTableRed[(dataRed1 & (0x7 << 9)) >> 12],
            colorTableRed[(dataRed1 & (0x7 << 12)) >> 9],
            colorTableRed[(dataRed1 & (0x7 << 15)) >> 6],
            colorTableRed[(dataRed1 & (0x7 << 18)) >> 3],
            colorTableRed[(dataRed1 & (0x7 << 21)) >> 0],
        };
        static_assert(std::extent_v<decltype(pixelsRed)> == BC4_BLOCK_PIXELS * BC4_BLOCK_PIXELS);

        for (auto curHeight = 0u; curHeight < height; curHeight++)
        {
            for (auto curWidth = 0u; curWidth < width; curWidth++)
            {
                out[curHeight * outPitch + curWidth * outPixelSize + outOffsetR] = pixelsRed[curHeight * BC4_BLOCK_PIXELS + curWidth];
            }
        }
    }
} // namespace

namespace image
{
    std::unique_ptr<Texture> DecompressorBc4::Decompress(const Texture& input, const ImageFormat* targetFormat)
    {
        assert(targetFormat->GetType() == ImageFormatType::UNSIGNED);
        if (targetFormat->GetType() != ImageFormatType::UNSIGNED)
            return nullptr;

        const auto* unsignedTargetFormat = dynamic_cast<const ImageFormatUnsigned*>(targetFormat);

        const auto outPixelSize = unsignedTargetFormat->m_bits_per_pixel / 8u;

        // Only support formats with byte aligned channels
        const auto redByteAligned = unsignedTargetFormat->m_r_size == 8 && (unsignedTargetFormat->m_r_offset % 8) == 0;
        assert(redByteAligned);
        if (!redByteAligned)
            return nullptr;

        const auto outOffsetR = unsignedTargetFormat->m_r_offset / 8;

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
                    const auto fullBlocksWidth = utils::AlignToPrevious(mipWidth, BC4_BLOCK_PIXELS);
                    const auto fullBlocksHeight = utils::AlignToPrevious(mipHeight, BC4_BLOCK_PIXELS);

                    for (auto curHeight = 0u; curHeight < fullBlocksHeight; curHeight += BC4_BLOCK_PIXELS)
                    {
                        for (auto curWidth = 0u; curWidth < fullBlocksWidth; curWidth += BC4_BLOCK_PIXELS)
                        {
                            DecompressBlock(bufferIn, bufferOut, mipPitch, outPixelSize, outOffsetR);
                            bufferIn += BC4_BLOCK_SIZE;
                            bufferOut += outPixelSize * BC4_BLOCK_PIXELS;
                        }

                        if (fullBlocksWidth < mipWidth)
                        {
                            const auto edgeBlockWidth = mipWidth - fullBlocksWidth;
                            DecompressBlockEdge(bufferIn, bufferOut, edgeBlockWidth, BC4_BLOCK_PIXELS, mipPitch, outPixelSize, outOffsetR);
                            bufferIn += BC4_BLOCK_SIZE;
                            bufferOut += outPixelSize * edgeBlockWidth;
                        }

                        bufferOut += mipPitch * (BC4_BLOCK_PIXELS - 1);
                    }

                    if (fullBlocksHeight < mipHeight)
                    {
                        const auto edgeBlockHeight = mipHeight - fullBlocksHeight;
                        for (auto curWidth = 0u; curWidth < fullBlocksWidth; curWidth += BC4_BLOCK_PIXELS)
                        {
                            DecompressBlockEdge(bufferIn, bufferOut, BC4_BLOCK_PIXELS, edgeBlockHeight, mipPitch, outPixelSize, outOffsetR);
                            bufferIn += BC4_BLOCK_SIZE;
                            bufferOut += outPixelSize * BC4_BLOCK_PIXELS;
                        }

                        if (fullBlocksWidth < mipWidth)
                        {
                            const auto edgeBlockWidth = mipWidth - fullBlocksWidth;
                            DecompressBlockEdge(bufferIn, bufferOut, edgeBlockWidth, edgeBlockHeight, mipPitch, outPixelSize, outOffsetR);
                            bufferIn += BC4_BLOCK_SIZE;
                            bufferOut += outPixelSize * edgeBlockWidth;
                        }

                        bufferOut += mipPitch * (BC4_BLOCK_PIXELS - 1);
                    }
                }
            }
        }

        return result;
    }
} // namespace image
