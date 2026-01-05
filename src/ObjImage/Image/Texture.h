#pragma once
#include "ImageFormat.h"

#include <cstdint>

namespace image
{
    enum class TextureType : std::uint8_t
    {
        T_2D,
        T_CUBE,
        T_3D
    };

    class Texture
    {
    protected:
        const ImageFormat* m_format;
        bool m_has_mip_maps;
        uint8_t* m_data;

        Texture(const ImageFormat* format, bool mipMaps);
        Texture(Texture&& other) noexcept;

        Texture& operator=(Texture&& other) noexcept;

    public:
        Texture(const Texture& other) = delete;
        virtual ~Texture();

        Texture& operator=(const Texture& other) = delete;

        [[nodiscard]] virtual TextureType GetTextureType() const = 0;
        [[nodiscard]] const ImageFormat* GetFormat() const;

        [[nodiscard]] virtual unsigned GetWidth() const = 0;
        [[nodiscard]] virtual unsigned GetHeight() const = 0;
        [[nodiscard]] virtual unsigned GetDepth() const = 0;
        [[nodiscard]] virtual int GetFaceCount() const = 0;

        void Allocate();
        [[nodiscard]] bool Empty() const;

        [[nodiscard]] virtual size_t GetSizeOfMipLevel(int mipLevel) const = 0;
        [[nodiscard]] virtual uint8_t* GetBufferForMipLevel(int mipLevel, int face) = 0;
        [[nodiscard]] virtual const uint8_t* GetBufferForMipLevel(int mipLevel, int face) const = 0;
        [[nodiscard]] uint8_t* GetBufferForMipLevel(int mipLevel);
        [[nodiscard]] const uint8_t* GetBufferForMipLevel(int mipLevel) const;

        [[nodiscard]] bool HasMipMaps() const;
        [[nodiscard]] virtual int GetMipMapCount() const = 0;
    };

    class Texture2D : public Texture
    {
    protected:
        unsigned m_width;
        unsigned m_height;

    public:
        Texture2D(const ImageFormat* format, unsigned width, unsigned height);
        Texture2D(const ImageFormat* format, unsigned width, unsigned height, bool mipMaps);
        Texture2D(const Texture2D& other) = delete;
        Texture2D(Texture2D&& other) noexcept;
        ~Texture2D() override;

        Texture2D& operator=(const Texture2D& other) = delete;
        Texture2D& operator=(Texture2D&& other) noexcept;

        [[nodiscard]] TextureType GetTextureType() const override;

        [[nodiscard]] unsigned GetWidth() const override;
        [[nodiscard]] unsigned GetHeight() const override;
        [[nodiscard]] unsigned GetDepth() const override;
        [[nodiscard]] int GetFaceCount() const override;

        [[nodiscard]] size_t GetSizeOfMipLevel(int mipLevel) const override;
        [[nodiscard]] uint8_t* GetBufferForMipLevel(int mipLevel, int face) override;
        [[nodiscard]] const uint8_t* GetBufferForMipLevel(int mipLevel, int face) const override;

        [[nodiscard]] int GetMipMapCount() const override;
    };

    class TextureCube final : public Texture2D
    {
        static const int FACE_COUNT;

    public:
        TextureCube(const ImageFormat* format, unsigned width, unsigned height);
        TextureCube(const ImageFormat* format, unsigned width, unsigned height, bool mipMaps);
        TextureCube(const TextureCube& other) = delete;
        TextureCube(TextureCube&& other) noexcept;
        ~TextureCube() override;

        TextureCube& operator=(const TextureCube& other) = delete;
        TextureCube& operator=(TextureCube&& other) noexcept;

        [[nodiscard]] TextureType GetTextureType() const override;

        [[nodiscard]] int GetFaceCount() const override;

        [[nodiscard]] uint8_t* GetBufferForMipLevel(int mipLevel, int face) override;
        [[nodiscard]] const uint8_t* GetBufferForMipLevel(int mipLevel, int face) const override;
    };

    class Texture3D final : public Texture
    {
        unsigned m_width;
        unsigned m_height;
        unsigned m_depth;

    public:
        Texture3D(const ImageFormat* format, unsigned width, unsigned height, unsigned depth);
        Texture3D(const ImageFormat* format, unsigned width, unsigned height, unsigned depth, bool mipMaps);
        Texture3D(const Texture3D& other) = delete;
        Texture3D(Texture3D&& other) noexcept;
        ~Texture3D() override;

        Texture3D& operator=(const Texture3D& other) = delete;
        Texture3D& operator=(Texture3D&& other) noexcept;

        [[nodiscard]] TextureType GetTextureType() const override;

        [[nodiscard]] unsigned GetWidth() const override;
        [[nodiscard]] unsigned GetHeight() const override;
        [[nodiscard]] unsigned GetDepth() const override;
        [[nodiscard]] int GetFaceCount() const override;

        [[nodiscard]] size_t GetSizeOfMipLevel(int mipLevel) const override;
        [[nodiscard]] uint8_t* GetBufferForMipLevel(int mipLevel, int face) override;
        [[nodiscard]] const uint8_t* GetBufferForMipLevel(int mipLevel, int face) const override;

        [[nodiscard]] int GetMipMapCount() const override;
    };
} // namespace image
