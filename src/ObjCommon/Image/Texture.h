#pragma once
#include "ImageFormat.h"
#include <cstdint>

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

    const ImageFormat* GetFormat() const;

    void Allocate();
    bool Empty() const;

    virtual size_t GetSizeOfMipLevel(int mipLevel) const = 0;
    virtual uint8_t* GetBufferForMipLevel(int mipLevel) = 0;

    bool HasMipMaps() const;
    virtual int GetMipMapCount() const = 0;
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

    unsigned GetWidth() const;
    unsigned GetHeight() const;

    size_t GetSizeOfMipLevel(int mipLevel) const override;
    uint8_t* GetBufferForMipLevel(int mipLevel) override;

    int GetMipMapCount() const override;
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

    size_t GetSizeOfMipLevel(int mipLevel) const override;
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

    unsigned GetWidth() const;
    unsigned GetHeight() const;
    unsigned GetDepth() const;

    size_t GetSizeOfMipLevel(int mipLevel) const override;
    uint8_t* GetBufferForMipLevel(int mipLevel) override;

    int GetMipMapCount() const override;
};