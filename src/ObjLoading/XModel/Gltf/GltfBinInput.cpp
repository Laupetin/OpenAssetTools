#include "GltfBinInput.h"

#include "XModel/Gltf/GltfConstants.h"

#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace gltf;

BinInput::BinInput()
    : m_buffer_size(0u)
{
}

bool BinInput::GetEmbeddedBuffer(const void*& buffer, size_t& bufferSize) const
{
    if (!m_buffer || !m_buffer_size)
        return false;

    buffer = m_buffer.get();
    bufferSize = m_buffer_size;

    return true;
}

const nlohmann::json& BinInput::GetJson() const
{
    if (!m_json)
        throw std::exception();

    return *m_json;
}

bool BinInput::ReadGltfData(std::istream& stream)
{
    uint32_t magic;
    if (!Read(stream, &magic, sizeof(magic), "magic"))
        return false;

    if (magic != GLTF_MAGIC)
    {
        std::cerr << "Invalid magic when trying to read GLB\n";
        return false;
    }

    uint32_t version;
    if (!Read(stream, &version, sizeof(version), "version"))
        return false;

    if (version != GLTF_VERSION)
    {
        std::cerr << std::format("Unsupported version {} when trying to read GLB: Expected version {}\n", version, GLTF_VERSION);
        return false;
    }

    uint32_t fileLength;
    if (!Read(stream, &fileLength, sizeof(fileLength), "file length"))
        return false;

    while (true)
    {
        uint32_t chunkLength;
        uint32_t chunkMagic;
        if (!Read(stream, &chunkLength, sizeof(chunkLength), "chunk length", false))
            break;

        if (!Read(stream, &chunkMagic, sizeof(chunkMagic), "chunk magic"))
            return false;

        if (chunkMagic == CHUNK_MAGIC_JSON)
        {
            const auto jsonBuffer = std::make_unique<uint8_t[]>(chunkLength + 1);
            if (!Read(stream, jsonBuffer.get(), chunkLength, "json"))
                return false;
            jsonBuffer[chunkLength] = 0u;

            try
            {
                m_json = std::make_unique<nlohmann::json>(nlohmann::json::parse(jsonBuffer.get(), &jsonBuffer[chunkLength]));
            }
            catch (const nlohmann::json::exception& e)
            {
                std::cerr << std::format("Failed trying to parse JSON of GLB: {}\n", e.what());
                return false;
            }
        }
        else if (chunkMagic == CHUNK_MAGIC_BIN)
        {
            m_buffer = std::make_unique<uint8_t[]>(chunkLength);
            m_buffer_size = chunkLength;

            if (!Read(stream, m_buffer.get(), m_buffer_size, "bin buffer"))
                return false;
        }
        else
            Skip(stream, chunkLength);

        if (chunkLength % 4u > 0)
            Skip(stream, 4u - (chunkLength % 4u));
    }

    if (!m_json)
    {
        std::cerr << "Failed to load GLB due to missing JSON\n";
        return false;
    }

    return true;
}

bool BinInput::Read(std::istream& stream, void* dest, const size_t dataSize, const char* readTypeName, const bool errorWhenFailed)
{
    stream.read(static_cast<char*>(dest), dataSize);
    if (stream.gcount() != dataSize)
    {
        if (errorWhenFailed)
            std::cerr << std::format("Unexpected EOF while reading GLB {}\n", readTypeName);
        return false;
    }

    return true;
}

void BinInput::Skip(std::istream& stream, const size_t skipLength)
{
    stream.seekg(skipLength, std::ios::cur);
}
