#include "TextureConverter.h"

#include <cassert>

constexpr uint64_t TextureConverter::Mask1(const unsigned length)
{
    if (length >= sizeof(uint64_t) * 8)
        return UINT64_MAX;

    return UINT64_MAX >> (sizeof(uint64_t) * 8 - length);
}

void TextureConverter::SetPixelFunctions(const unsigned inBitCount, const unsigned outBitCount)
{
    switch (inBitCount)
    {
    case 16:
        m_read_pixel_func = [](const void* offset, unsigned bitCount)
        {
            return static_cast<uint64_t>(*static_cast<const uint16_t*>(offset));
        };
        break;

    case 32:
        m_read_pixel_func = [](const void* offset, unsigned bitCount)
        {
            return static_cast<uint64_t>(*static_cast<const uint32_t*>(offset));
        };
        break;

    case 64:
        m_read_pixel_func = [](const void* offset, unsigned bitCount)
        {
            return *static_cast<const uint64_t*>(offset);
        };
        break;

    default:
        if (inBitCount <= 64)
        {
            m_read_pixel_func = [](const void* offset, const unsigned bitCount)
            {
                uint64_t result = 0;

                for (auto pixelOffset = 0u; pixelOffset < bitCount; pixelOffset += 8)
                {
                    result |= (static_cast<uint64_t>(*(static_cast<const uint8_t*>(offset) + (pixelOffset / 8))) <<
                        pixelOffset);
                }

                return result;
            };
        }
        else
        {
            assert(false);
            m_read_pixel_func = [](const void* offset, unsigned bitCount)
            {
                return 0ui64;
            };
        }
        break;
    }

    switch (outBitCount)
    {
    case 16:
        m_write_pixel_func = [](void* offset, const uint64_t pixel, unsigned bitCount)
        {
            *static_cast<uint16_t*>(offset) = static_cast<uint16_t>(pixel);
        };
        break;

    case 32:
        m_write_pixel_func = [](void* offset, const uint64_t pixel, unsigned bitCount)
        {
            *static_cast<uint32_t*>(offset) = static_cast<uint32_t>(pixel);
        };
        break;

    case 64:
        m_write_pixel_func = [](void* offset, const uint64_t pixel, unsigned bitCount)
        {
            *static_cast<uint64_t*>(offset) = pixel;
        };
        break;

    default:
        if (inBitCount <= 64)
        {
            m_write_pixel_func = [](void* offset, const uint64_t pixel, const unsigned bitCount)
            {
                for (auto pixelOffset = 0u; pixelOffset < bitCount; pixelOffset += 8)
                {
                    *(static_cast<uint8_t*>(offset) + (pixelOffset / 8)) = static_cast<uint8_t>(pixel >> pixelOffset);
                }
            };
        }
        else
        {
            assert(false);
            m_write_pixel_func = [](void* offset, uint64_t pixel, unsigned bitCount)
            {
            };
        }
        break;
    }
}

TextureConverter::TextureConverter(Texture* inputTexture, const ImageFormat* targetFormat)
    : m_input_texture(inputTexture),
      m_output_texture(nullptr),
      m_input_format(inputTexture->GetFormat()),
      m_output_format(targetFormat)
{
}

void TextureConverter::CreateOutputTexture()
{
    switch (m_input_texture->GetTextureType())
    {
    case TextureType::T_2D:
        m_output_texture = new Texture2D(m_output_format, m_input_texture->GetWidth(), m_input_texture->GetHeight(),
                                         m_input_texture->HasMipMaps());
        break;

    case TextureType::T_CUBE:
        m_output_texture = new TextureCube(m_output_format, m_input_texture->GetWidth(), m_input_texture->GetHeight(),
                                           m_input_texture->HasMipMaps());
        break;

    case TextureType::T_3D:
        m_output_texture = new Texture3D(m_output_format, m_input_texture->GetWidth(), m_input_texture->GetHeight(),
                                         m_input_texture->GetDepth(), m_input_texture->HasMipMaps());
        break;
    default:
        assert(false);
        break;
    }

    m_output_texture->Allocate();
}

void TextureConverter::ReorderUnsignedToUnsigned() const
{
    const auto* inputFormat = dynamic_cast<const ImageFormatUnsigned*>(m_input_format);
    const auto* outputFormat = dynamic_cast<const ImageFormatUnsigned*>(m_output_format);
    const auto mipCount = m_input_texture->HasMipMaps() ? m_input_texture->GetMipMapCount() : 1;

    const auto rInputMask = inputFormat->HasR() ? Mask1(inputFormat->m_r_size) << inputFormat->m_r_offset : 0;
    const auto gInputMask = inputFormat->HasG() ? Mask1(inputFormat->m_g_size) << inputFormat->m_g_offset : 0;
    const auto bInputMask = inputFormat->HasB() ? Mask1(inputFormat->m_b_size) << inputFormat->m_b_offset : 0;
    const auto aInputMask = inputFormat->HasA() ? Mask1(inputFormat->m_a_size) << inputFormat->m_a_offset : 0;
    const bool rConvert = rInputMask != 0 && outputFormat->m_r_size > 0;
    const bool gConvert = gInputMask != 0 && outputFormat->m_g_size > 0;
    const bool bConvert = bInputMask != 0 && outputFormat->m_b_size > 0;
    const bool aConvert = aInputMask != 0 && outputFormat->m_a_size > 0;

    for (auto mipLevel = 0; mipLevel < mipCount; mipLevel++)
    {
        const auto mipLevelSize = m_input_texture->GetSizeOfMipLevel(mipLevel);
        const auto* inputBuffer = m_input_texture->GetBufferForMipLevel(mipLevel);
        auto* outputBuffer = m_output_texture->GetBufferForMipLevel(mipLevel);

        const auto inputBytePerPixel = inputFormat->m_bits_per_pixel / 8;
        const auto outputBytePerPixel = outputFormat->m_bits_per_pixel / 8;

        auto outputOffset = 0u;
        for (auto inputOffset = 0u; inputOffset < mipLevelSize; inputOffset += inputBytePerPixel, outputOffset +=
             outputBytePerPixel)
        {
            uint64_t outPixel = 0;
            const auto inPixel = m_read_pixel_func(&inputBuffer[inputOffset], inputFormat->m_bits_per_pixel);

            if (rConvert)
                outPixel |= (inPixel & rInputMask) >> inputFormat->m_r_offset << outputFormat->m_r_offset;
            if (gConvert)
                outPixel |= (inPixel & gInputMask) >> inputFormat->m_g_offset << outputFormat->m_g_offset;
            if (bConvert)
                outPixel |= (inPixel & bInputMask) >> inputFormat->m_b_offset << outputFormat->m_b_offset;
            if (aConvert)
                outPixel |= (inPixel & aInputMask) >> inputFormat->m_a_offset << outputFormat->m_a_offset;

            m_write_pixel_func(&outputBuffer[outputOffset], outPixel, outputFormat->m_bits_per_pixel);
        }
    }
}

void TextureConverter::ConvertUnsignedToUnsigned()
{
    const auto* inputFormat = dynamic_cast<const ImageFormatUnsigned*>(m_input_format);
    const auto* outputFormat = dynamic_cast<const ImageFormatUnsigned*>(m_output_format);

    assert(inputFormat->m_bits_per_pixel <= 64);
    assert(outputFormat->m_bits_per_pixel <= 64);

    SetPixelFunctions(inputFormat->m_bits_per_pixel, outputFormat->m_bits_per_pixel);

    if (inputFormat->m_r_size == outputFormat->m_r_size
        && inputFormat->m_g_size == outputFormat->m_g_size
        && inputFormat->m_b_size == outputFormat->m_b_size
        && inputFormat->m_a_size == outputFormat->m_a_size)
    {
        ReorderUnsignedToUnsigned();
    }
    else
    {
        // Unsupported as of now
        assert(false);
    }
}

Texture* TextureConverter::Convert()
{
    CreateOutputTexture();

    if (m_input_format->GetType() == ImageFormatType::UNSIGNED
        && m_output_format->GetType() == ImageFormatType::UNSIGNED)
    {
        ConvertUnsignedToUnsigned();
    }
    else
    {
        // Unsupported as of now
        assert(false);
    }

    return m_output_texture;
}
