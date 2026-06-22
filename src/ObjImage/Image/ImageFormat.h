#pragma once

#include "D3DFormat.h"
#include "DxgiFormat.h"

#include <cstddef>
#include <cstdint>
#include <utility>

namespace image
{
    enum class ImageFormatId : std::uint8_t
    {
        R8_G8_B8,
        B8_G8_R8,
        B8_G8_R8_X8,
        R8_G8_B8_A8,
        B8_G8_R8_A8,
        A8,
        R16_G16_B16_A16_FLOAT,
        R8,
        R8_A8,
        BC1,
        BC2,
        BC3,
        BC4,
        BC5,

        MAX
    };

    enum class ImageFormatType : std::uint8_t
    {
        UNKNOWN,
        UNSIGNED,
        BLOCK_COMPRESSED
    };

    class ImageFormatUnsigned;
    class ImageFormatBlockCompressed;

    class ImageFormat
    {
    public:
        static const ImageFormat* GetImageFormatById(ImageFormatId id);
        virtual ~ImageFormat() = default;

        [[nodiscard]] ImageFormatId GetId() const;
        [[nodiscard]] const std::string& GetName() const;
        [[nodiscard]] oat::D3DFORMAT GetD3DFormat() const;
        [[nodiscard]] oat::DXGI_FORMAT GetDxgiFormat() const;

        [[nodiscard]] virtual ImageFormatType GetType() const = 0;
        [[nodiscard]] virtual size_t GetPitch(unsigned mipLevel, unsigned width) const = 0;
        [[nodiscard]] virtual size_t GetSizeOfMipLevel(unsigned mipLevel, unsigned width, unsigned height, unsigned depth) const = 0;

        [[nodiscard]] virtual bool HasR() const = 0;
        [[nodiscard]] virtual bool HasG() const = 0;
        [[nodiscard]] virtual bool HasB() const = 0;
        [[nodiscard]] virtual bool HasA() const = 0;

    protected:
        ImageFormat(ImageFormatId id, std::string name, oat::D3DFORMAT d3dFormat, oat::DXGI_FORMAT dxgiFormat);

    private:
        ImageFormatId m_id;
        std::string m_name;
        oat::D3DFORMAT m_d3d_format;
        oat::DXGI_FORMAT m_dxgi_format;
    };

    class ImageFormatUnsigned final : public ImageFormat
    {
    public:
        ImageFormatUnsigned(ImageFormatId id,
                            std::string name,
                            oat::D3DFORMAT d3dFormat,
                            oat::DXGI_FORMAT dxgiFormat,
                            unsigned bitsPerPixel,
                            unsigned rOffset,
                            unsigned rSize,
                            unsigned gOffset,
                            unsigned gSize,
                            unsigned bOffset,
                            unsigned bSize,
                            unsigned aOffset,
                            unsigned aSize);

        [[nodiscard]] ImageFormatType GetType() const override;
        [[nodiscard]] size_t GetPitch(unsigned mipLevel, unsigned width) const override;
        [[nodiscard]] size_t GetSizeOfMipLevel(unsigned mipLevel, unsigned width, unsigned height, unsigned depth) const override;

        [[nodiscard]] bool HasR() const override;
        [[nodiscard]] bool HasG() const override;
        [[nodiscard]] bool HasB() const override;
        [[nodiscard]] bool HasA() const override;

        unsigned m_bits_per_pixel;
        unsigned m_r_offset;
        unsigned m_r_size;
        unsigned m_g_offset;
        unsigned m_g_size;
        unsigned m_b_offset;
        unsigned m_b_size;
        unsigned m_a_offset;
        unsigned m_a_size;
    };

    class ImageFormatBlockCompressed final : public ImageFormat
    {
    public:
        ImageFormatBlockCompressed(ImageFormatId id,
                                   std::string name,
                                   oat::D3DFORMAT d3dFormat,
                                   oat::DXGI_FORMAT dxgiFormat,
                                   unsigned blockSize,
                                   unsigned bitsPerBlock,
                                   bool hasR,
                                   bool hasG,
                                   bool hasB,
                                   bool hasA);

        [[nodiscard]] ImageFormatType GetType() const override;
        [[nodiscard]] size_t GetPitch(unsigned mipLevel, unsigned width) const override;
        [[nodiscard]] size_t GetSizeOfMipLevel(unsigned mipLevel, unsigned width, unsigned height, unsigned depth) const override;

        [[nodiscard]] bool HasR() const override;
        [[nodiscard]] bool HasG() const override;
        [[nodiscard]] bool HasB() const override;
        [[nodiscard]] bool HasA() const override;

        unsigned m_block_size;
        unsigned m_bits_per_block;
        bool m_has_r;
        bool m_has_g;
        bool m_has_b;
        bool m_has_a;
    };

    namespace format
    {
        extern const ImageFormatUnsigned R8_G8_B8;
        extern const ImageFormatUnsigned B8_G8_R8;
        extern const ImageFormatUnsigned B8_G8_R8_X8;
        extern const ImageFormatUnsigned R8_G8_B8_A8;
        extern const ImageFormatUnsigned B8_G8_R8_A8;
        extern const ImageFormatUnsigned A8;
        extern const ImageFormatUnsigned R16_G16_B16_A16_FLOAT; // TODO: Float not unsigned
        extern const ImageFormatUnsigned R8;
        extern const ImageFormatUnsigned R8_A8;
        extern const ImageFormatBlockCompressed BC1;
        extern const ImageFormatBlockCompressed BC2;
        extern const ImageFormatBlockCompressed BC3;
        extern const ImageFormatBlockCompressed BC4;
        extern const ImageFormatBlockCompressed BC5;

        extern const ImageFormat* const ALL[std::to_underlying(ImageFormatId::MAX)];
    } // namespace format
} // namespace image
