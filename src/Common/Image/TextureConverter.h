#pragma once

#include "Texture.h"

#include <functional>

class TextureConverter
{
    Texture* m_input_texture;
    Texture* m_output_texture;
    const ImageFormat* m_input_format;
    const ImageFormat* m_output_format;

    std::function<uint64_t(const void* offset, unsigned bitCount)> m_read_pixel_func;
    std::function<void(void* offset, uint64_t pixel, unsigned bitCount)> m_write_pixel_func;

    static constexpr uint64_t Mask1(unsigned length);
    void SetPixelFunctions(unsigned inBitCount, unsigned outBitCount);

    void CreateOutputTexture();

    void ReorderUnsignedToUnsigned() const;
    void ConvertUnsignedToUnsigned();

public:
    TextureConverter(Texture* inputTexture, const ImageFormat* targetFormat);

    Texture* Convert();
};
