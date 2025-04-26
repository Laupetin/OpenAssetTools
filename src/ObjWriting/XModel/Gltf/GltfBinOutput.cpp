#include "GltfBinOutput.h"

#include "Utils/Alignment.h"
#include "XModel/Gltf/GltfConstants.h"

#include <cstdint>
#include <nlohmann/json.hpp>

using namespace gltf;

BinOutput::BinOutput(std::ostream& stream)
    : m_stream(stream)
{
}

void BinOutput::Write(const void* data, const size_t dataSize) const
{
    m_stream.write(static_cast<const char*>(data), dataSize);
}

void BinOutput::AlignToFour(const char value) const
{
    const auto offset = m_stream.tellp();
    if (offset % 4 > 0)
    {
        const uint32_t alignmentValue = FileUtils::MakeMagic32(value, value, value, value);
        Write(&alignmentValue, 4u - (offset % 4u));
    }
}

std::optional<std::string> BinOutput::CreateBufferUri(const void* buffer, size_t bufferSize) const
{
    return std::nullopt;
}

void BinOutput::EmitJson(const nlohmann::json& json) const
{
    static constexpr uint32_t ZERO = 0u;

    Write(&GLTF_MAGIC, sizeof(GLTF_MAGIC));
    Write(&GLTF_VERSION, sizeof(GLTF_VERSION));

    // File length will be filled later
    Write(&ZERO, sizeof(ZERO));

    // Chunk length will be filled after writing json
    Write(&ZERO, sizeof(ZERO));
    Write(&CHUNK_MAGIC_JSON, sizeof(CHUNK_MAGIC_JSON));

    m_stream << json;

    AlignToFour(' ');

    const auto offsetAfterData = m_stream.tellp();
    const auto jsonDataLength = static_cast<uint32_t>(static_cast<unsigned>(offsetAfterData) - GLTF_JSON_CHUNK_DATA_OFFSET);

    // Fill chunk length now
    m_stream.seekp(GLTF_JSON_CHUNK_LENGTH_OFFSET, std::ios::beg);
    Write(&jsonDataLength, sizeof(jsonDataLength));

    // Return to previous pos
    m_stream.seekp(offsetAfterData, std::ios::beg);
}

void BinOutput::EmitBuffer(const void* buffer, const size_t bufferSize) const
{
    const auto chunkLength = utils::Align<uint32_t>(static_cast<uint32_t>(bufferSize), 4u);
    Write(&chunkLength, sizeof(chunkLength));
    Write(&CHUNK_MAGIC_BIN, sizeof(CHUNK_MAGIC_BIN));

    Write(buffer, bufferSize);
    AlignToFour('\0');
}

void BinOutput::Finalize() const
{
    const auto fileEndOffset = m_stream.tellp();
    const auto fileSize = static_cast<uint32_t>(fileEndOffset);

    // Fill chunk length now
    m_stream.seekp(GLTF_LENGTH_OFFSET, std::ios::beg);
    Write(&fileSize, sizeof(fileSize));

    // Return to file end
    m_stream.seekp(fileEndOffset, std::ios::beg);
}
