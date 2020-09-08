#pragma once
#include "ImageFormat.h"
#include <cstdint>

enum class TextureType
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

    virtual TextureType GetTextureType() const = 0;
    const ImageFormat* GetFormat() const;

    virtual unsigned GetWidth() const = 0;
    virtual unsigned GetHeight() const = 0;
    virtual unsigned GetDepth() const = 0;
    virtual int GetFaceCount() const = 0;

    void Allocate();
    bool Empty() const;

    virtual size_t GetSizeOfMipLevel(int mipLevel) const = 0;
    virtual uint8_t* GetBufferForMipLevel(int mipLevel, int face) = 0;
    uint8_t* GetBufferForMipLevel(int mipLevel);

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

    TextureType GetTextureType() const override;

    unsigned GetWidth() const override;
    unsigned GetHeight() const override;
    unsigned GetDepth() const override;
    int GetFaceCount() const override;

    size_t GetSizeOfMipLevel(int mipLevel) const override;
    uint8_t* GetBufferForMipLevel(int mipLevel, int face) override;

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

    TextureType GetTextureType() const override;

    int GetFaceCount() const override;

    uint8_t* GetBufferForMipLevel(int mipLevel, int face) override;
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

    TextureType GetTextureType() const override;

    unsigned GetWidth() const override;
    unsigned GetHeight() const override;
    unsigned GetDepth() const override;
    int GetFaceCount() const override;

    size_t GetSizeOfMipLevel(int mipLevel) const override;
    uint8_t* GetBufferForMipLevel(int mipLevel, int face) override;

    int GetMipMapCount() const override;
};