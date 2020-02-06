#include "Texture.h"
#include <algorithm>
#include <cassert>

// ==============================================
// ================= Texture ====================
// ==============================================
Texture::Texture(const IImageFormat* format, const bool mipMaps)
{
    m_format = format;
    m_has_mip_maps = mipMaps;
    m_data = nullptr;
}

Texture::Texture(Texture&& other) noexcept
{
    m_format = other.m_format;
    m_has_mip_maps = other.m_has_mip_maps;
    m_data = other.m_data;

    other.m_data = nullptr;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    m_format = other.m_format;
    m_has_mip_maps = other.m_has_mip_maps;
    m_data = other.m_data;

    other.m_data = nullptr;

    return *this;
}

Texture::~Texture()
{
    delete[] m_data;
    m_data = nullptr;
}

const IImageFormat* Texture::GetFormat() const
{
    return m_format;
}

void Texture::Allocate()
{
    size_t storageRequirement = 0;
    const int mipLevelCount = m_has_mip_maps ? GetMipMapCount() : 1;

    for (int currentMipLevel = 0; currentMipLevel < mipLevelCount; currentMipLevel++)
    {
        storageRequirement += GetSizeOfMipLevel(currentMipLevel);
    }

    if (storageRequirement > 0)
    {
        m_data = new uint8_t[storageRequirement];
        memset(m_data, 0, storageRequirement);
    }
}

bool Texture::Empty() const
{
    return m_data == nullptr;
}

bool Texture::HasMipMaps() const
{
    return m_has_mip_maps;
}

// ==============================================
// ================ Texture2D ===================
// ==============================================
Texture2D::Texture2D(const IImageFormat* format, const unsigned width, const unsigned height)
    : Texture2D(format, width, height, false)
{
}

Texture2D::Texture2D(const IImageFormat* format, const unsigned width, const unsigned height, const bool mipMaps)
    : Texture(format, mipMaps)
{
    m_width = width;
    m_height = height;
}

Texture2D::Texture2D(Texture2D&& other) noexcept
    : Texture(std::move(other)),
      m_width(other.m_width),
      m_height(other.m_height)
{
}

Texture2D::~Texture2D() = default;

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
    if (this == &other)
        return *this;

    Texture::operator=(std::move(other));
    m_width = other.m_width;
    m_height = other.m_height;

    return *this;
}

unsigned Texture2D::GetWidth() const
{
    return m_width;
}

unsigned Texture2D::GetHeight() const
{
    return m_height;
}

size_t Texture2D::GetSizeOfMipLevel(const int mipLevel) const
{
    return m_format->GetSizeOfMipLevel(mipLevel, m_width, m_height, 1);
}

int Texture2D::GetMipMapCount() const
{
    unsigned maxDimension = std::max(m_width, m_height);

    int mipMapCount = 0;

    while (maxDimension != 0)
    {
        maxDimension >>= 1;
        mipMapCount++;
    }

    return mipMapCount;
}

uint8_t* Texture2D::GetBufferForMipLevel(const int mipLevel)
{
    assert(mipLevel >= 0);
    assert(mipLevel < (m_has_mip_maps ? GetMipMapCount() : 1));
    assert(m_data);

    if (mipLevel < 0 || mipLevel >= (m_has_mip_maps ? GetMipMapCount() : 1))
        return nullptr;

    if (!m_data)
        return nullptr;

    size_t bufferOffset = 0;
    for (int previousMipLevel = 0; previousMipLevel < mipLevel; previousMipLevel++)
    {
        bufferOffset += GetSizeOfMipLevel(previousMipLevel);
    }

    return &m_data[bufferOffset];
}

// ==============================================
// =============== TextureCube ==================
// ==============================================
const int TextureCube::FACE_COUNT = 6;

TextureCube::TextureCube(const IImageFormat* format, const unsigned width, const unsigned height)
    : Texture2D(format, width, height)
{
}

TextureCube::TextureCube(const IImageFormat* format, const unsigned width, const unsigned height, const bool mipMaps)
    : Texture2D(format, width, height, mipMaps)
{
}

TextureCube::TextureCube(TextureCube&& other) noexcept
    : Texture2D(std::move(other))
{
}

TextureCube::~TextureCube() = default;

TextureCube& TextureCube::operator=(TextureCube&& other) noexcept
{
    if (this == &other)
        return *this;

    Texture2D::operator=(std::move(other));

    return *this;
}

size_t TextureCube::GetSizeOfMipLevel(const int mipLevel) const
{
    return m_format->GetSizeOfMipLevel(mipLevel, m_width, m_height, 1) * FACE_COUNT;
}

// ==============================================
// ================ Texture3D ===================
// ==============================================

Texture3D::Texture3D(const IImageFormat* format, const unsigned width, const unsigned height, const unsigned depth)
    : Texture3D(format, width, height, depth, false)
{
}

Texture3D::Texture3D(const IImageFormat* format, const unsigned width, const unsigned height, const unsigned depth,
                     const bool mipMaps)
    : Texture(format, mipMaps)
{
    m_width = width;
    m_height = height;
    m_depth = depth;
}

Texture3D::Texture3D(Texture3D&& other) noexcept
    : Texture(std::move(other)),
      m_width(other.m_width),
      m_height(other.m_height),
      m_depth(other.m_depth)
{
}

Texture3D::~Texture3D() = default;

Texture3D& Texture3D::operator=(Texture3D&& other) noexcept
{
    if (this == &other)
        return *this;

    Texture::operator=(std::move(other));
    m_width = other.m_width;
    m_height = other.m_height;
    m_depth = other.m_depth;

    return *this;
}

unsigned Texture3D::GetWidth() const
{
    return m_width;
}

unsigned Texture3D::GetHeight() const
{
    return m_height;
}

unsigned Texture3D::GetDepth() const
{
    return m_depth;
}

size_t Texture3D::GetSizeOfMipLevel(const int mipLevel) const
{
    return m_format->GetSizeOfMipLevel(mipLevel, m_width, m_height, m_depth);
}

int Texture3D::GetMipMapCount() const
{
    unsigned maxDimension = std::max(std::max(m_width, m_height), m_depth);

    int mipMapCount = 0;

    while (maxDimension != 0)
    {
        maxDimension >>= 1;
        mipMapCount++;
    }

    return mipMapCount;
}

uint8_t* Texture3D::GetBufferForMipLevel(const int mipLevel)
{
    assert(mipLevel >= 0);
    assert(mipLevel < (m_has_mip_maps ? GetMipMapCount() : 1));
    assert(m_data);

    if (mipLevel < 0 || mipLevel >= (m_has_mip_maps ? GetMipMapCount() : 1))
        return nullptr;

    if (!m_data)
        return nullptr;

    size_t bufferOffset = 0;
    for (int previousMipLevel = 0; previousMipLevel < mipLevel; previousMipLevel++)
    {
        bufferOffset += GetSizeOfMipLevel(previousMipLevel);
    }

    return &m_data[bufferOffset];
}
