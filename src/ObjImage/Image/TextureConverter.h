#pragma once

#include "Texture.h"

#include <functional>
#include <memory>

class TextureConverter
{
public:
    TextureConverter(const Texture* inputTexture, const ImageFormat* targetFormat);

    std::unique_ptr<Texture> Convert();

private:
    static constexpr uint64_t Mask1(unsigned length);
    void SetPixelFunctions(unsigned inBitCount, unsigned outBitCount);

    void CreateOutputTexture();

    void ReorderUnsignedToUnsigned() const;
    void ConvertUnsignedToUnsigned();

    std::function<uint64_t(const void* offset, unsigned bitCount)> m_read_pixel_func;
    std::function<void(void* offset, uint64_t pixel, unsigned bitCount)> m_write_pixel_func;

    const Texture* m_input_texture;
    std::unique_ptr<Texture> m_output_texture;
    const ImageFormat* m_input_format;
    const ImageFormat* m_output_format;
};
