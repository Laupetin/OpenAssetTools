#include "GltfBuffer.h"

#include "XModel/Gltf/GltfConstants.h"

#include <cassert>
#include <cstdint>
#include <cstring>

#define LTC_NO_PROTOTYPES
#include <tomcrypt.h>

using namespace gltf;

EmbeddedBuffer::EmbeddedBuffer(const void* data, const size_t dataSize)
    : m_data(static_cast<const uint8_t*>(data)),
      m_data_size(dataSize)
{
}

bool EmbeddedBuffer::ReadData(void* dest, const size_t offset, const size_t count) const
{
    assert(m_data);
    assert(m_data_size > 0u);

    if (offset + count > m_data_size)
        return false;

    std::memcpy(dest, &m_data[offset], count);
    return true;
}

size_t EmbeddedBuffer::GetSize() const
{
    return m_data_size;
}

DataUriBuffer::DataUriBuffer()
    : m_data(nullptr),
      m_data_size(0u)
{
}

bool DataUriBuffer::IsDataUri(const std::string& uri)
{
    return uri.starts_with(GLTF_DATA_URI_PREFIX) && uri.size() > URI_PREFIX_LENGTH;
}

bool DataUriBuffer::ReadDataFromUri(const std::string& uri)
{
    if (!IsDataUri(uri))
        return false;

    const auto base64DataLength = uri.size() - URI_PREFIX_LENGTH;

    unsigned long outLength = base64DataLength / 4u;
    m_data = std::make_unique<uint8_t[]>(outLength);
    const auto result = base64_decode(&uri[URI_PREFIX_LENGTH], base64DataLength, m_data.get(), &outLength);
    m_data_size = static_cast<size_t>(outLength);

    assert(result == CRYPT_OK);

    return false;
}

bool DataUriBuffer::ReadData(void* dest, const size_t offset, const size_t count) const
{
    assert(m_data);
    assert(m_data_size > 0u);

    if (offset + count > m_data_size)
        return false;

    std::memcpy(dest, &m_data[offset], count);
    return true;
}

size_t DataUriBuffer::GetSize() const
{
    return m_data_size;
}
