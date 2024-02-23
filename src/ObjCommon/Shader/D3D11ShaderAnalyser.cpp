#include "D3D11ShaderAnalyser.h"

#include "Utils/FileUtils.h"

#include <cstring>

using namespace d3d11;

namespace d3d11
{
    static constexpr auto TAG_RDEF = FileUtils::MakeMagic32('R', 'D', 'E', 'F');
    static constexpr auto TAG_SHDR = FileUtils::MakeMagic32('S', 'H', 'D', 'R');

    static constexpr auto VERSION_5_0 = 0x500;
    static constexpr auto VERSION_5_1 = 0x501;
    static constexpr auto TARGET_VERSION_MASK = 0xFFFF;
    static constexpr auto CHUNK_TABLE_OFFSET = 28u;

    struct FileRdefHeader
    {
        uint32_t constantBufferCount;
        uint32_t constantBufferOffset;
        uint32_t boundResourceCount;
        uint32_t boundResourceOffset;
        uint32_t target;
        uint32_t flags;
        uint32_t creatorOffset;
    };

    struct FileRdefExtraHeader_5_0
    {
        // Wine project does not seem to know what this is
        uint32_t unknown[8];
    };

    static_assert(sizeof(FileRdefHeader) == 28);
    static_assert(sizeof(FileRdefExtraHeader_5_0) == 32);

    struct FileBoundResource
    {
        uint32_t nameOffset;
        uint32_t type;
        uint32_t returnType;
        uint32_t dimension;
        uint32_t numSamples;
        uint32_t bindPoint;
        uint32_t bindCount;
        uint32_t uFlags;
    };

    struct FileBoundResource_5_1 : FileBoundResource
    {
        uint32_t space;
        uint32_t uID;
    };

    static_assert(sizeof(FileBoundResource) == 32);
    static_assert(sizeof(FileBoundResource_5_1) == 40);

    struct FileConstantBuffer
    {
        uint32_t nameOffset;
        uint32_t variableCount;
        uint32_t variableOffset;
        uint32_t size;
        uint32_t flags;
        uint32_t type;
    };

    struct FileConstantBufferVariable
    {
        uint32_t nameOffset;
        uint32_t startOffset;
        uint32_t size;
        uint32_t flags;
        uint32_t typeOffset;
        uint32_t defaultValueOffset;
    };

    struct FileConstantBufferVariable_5_0 : FileConstantBufferVariable
    {
        // Wine project does not seem to know what this is
        uint32_t unknown[4];
    };

    static_assert(sizeof(FileConstantBufferVariable) == 24);
    static_assert(sizeof(FileConstantBufferVariable_5_0) == 40);

    enum FileProgramType : uint32_t
    {
        D3D10_SB_PIXEL_SHADER = 0,
        D3D10_SB_VERTEX_SHADER = 1,
        D3D10_SB_GEOMETRY_SHADER = 2,

        // D3D11 Shaders
        D3D11_SB_HULL_SHADER = 3,
        D3D11_SB_DOMAIN_SHADER = 4,
        D3D11_SB_COMPUTE_SHADER = 5,

        D3D11_SB_RESERVED0 = 0xFFF0
    };

    struct FileShaderHeader
    {
        uint32_t versionMinor : 4;
        uint32_t versionMajor : 4;
        uint32_t unused : 8;
        FileProgramType programType : 16;
    };

    static_assert(sizeof(FileShaderHeader) == 4);

    uint32_t ReadU32(const uint8_t*& ptr)
    {
        const auto result = *reinterpret_cast<const uint32_t*>(ptr);
        ptr += sizeof(uint32_t);
        return result;
    }

    bool FindChunk(const uint32_t magic, const uint8_t* byteCode, const size_t byteCodeSize, size_t& chunkOffset, size_t& chunkSize)
    {
        if (byteCodeSize < CHUNK_TABLE_OFFSET + sizeof(uint32_t))
            return false;

        const auto* ptr = byteCode + CHUNK_TABLE_OFFSET;
        const auto chunkCount = ReadU32(ptr);
        if (byteCodeSize < (ptr - byteCode) + chunkCount * sizeof(uint32_t))
            return false;

        for (auto chunkIndex = 0u; chunkIndex < chunkCount; chunkIndex++)
        {
            const auto currentChunkOffset = ReadU32(ptr);
            if (byteCodeSize < currentChunkOffset + sizeof(uint32_t) * 2)
                return false;

            const auto currentChunkTag = *reinterpret_cast<const uint32_t*>(byteCode + currentChunkOffset + 0);
            const auto currentChunkSize = *reinterpret_cast<const uint32_t*>(byteCode + currentChunkOffset + 4);

            if (byteCodeSize < currentChunkOffset + sizeof(uint32_t) * 2 + currentChunkSize)
                return false;

            if (currentChunkTag == magic)
            {
                chunkOffset = currentChunkOffset + 8;
                chunkSize = currentChunkSize;
                return true;
            }
        }

        return false;
    }

    bool StringFitsInChunk(const char* str, const uint8_t* shaderByteCode, const size_t shaderByteCodeSize)
    {
        const auto strLen = strnlen(str, shaderByteCodeSize - (reinterpret_cast<const uint8_t*>(str) - shaderByteCode));
        return str[strLen] == '\0';
    }

    bool PopulateBoundResource(BoundResource& boundResource,
                               const FileBoundResource& fileBoundResource,
                               const uint8_t* shaderByteCode,
                               const size_t shaderByteCodeSize,
                               const size_t chunkOffset)
    {
        const auto nameString = reinterpret_cast<const char*>(shaderByteCode + chunkOffset + fileBoundResource.nameOffset);

        if (!StringFitsInChunk(nameString, shaderByteCode, shaderByteCodeSize))
            return false;

        boundResource.m_name = std::string(nameString);
        boundResource.m_type = fileBoundResource.type;
        boundResource.m_return_type = fileBoundResource.returnType;
        boundResource.m_dimension = fileBoundResource.dimension;
        boundResource.m_num_samples = fileBoundResource.numSamples;
        boundResource.m_bind_point = fileBoundResource.bindPoint;
        boundResource.m_bind_count = fileBoundResource.bindCount;
        boundResource.m_flags = fileBoundResource.uFlags;

        return true;
    }

    bool PopulateConstantBufferVariable(ConstantBufferVariable& constantBufferVariable,
                                        const FileConstantBufferVariable& fileConstantBufferVariable,
                                        const uint8_t* shaderByteCode,
                                        const size_t shaderByteCodeSize,
                                        const size_t chunkOffset,
                                        const size_t chunkSize)
    {
        const auto nameString = reinterpret_cast<const char*>(shaderByteCode + chunkOffset + fileConstantBufferVariable.nameOffset);

        if (!StringFitsInChunk(nameString, shaderByteCode, shaderByteCodeSize))
            return false;

        constantBufferVariable.m_name = std::string(nameString);
        constantBufferVariable.m_offset = fileConstantBufferVariable.startOffset;
        constantBufferVariable.m_size = fileConstantBufferVariable.size;
        constantBufferVariable.m_flags = fileConstantBufferVariable.flags;

        return true;
    }

    bool PopulateConstantBuffer(ConstantBuffer& constantBuffer,
                                const FileConstantBuffer& fileConstantBuffer,
                                const uint8_t* shaderByteCode,
                                const size_t shaderByteCodeSize,
                                const size_t chunkOffset,
                                const size_t chunkSize,
                                const unsigned targetVersion)
    {
        const auto nameString = reinterpret_cast<const char*>(shaderByteCode + chunkOffset + fileConstantBuffer.nameOffset);

        if (!StringFitsInChunk(nameString, shaderByteCode, shaderByteCodeSize))
            return false;

        constantBuffer.m_name = std::string(nameString);

        constantBuffer.m_size = fileConstantBuffer.size;
        constantBuffer.m_flags = fileConstantBuffer.flags;
        constantBuffer.m_type = fileConstantBuffer.type;

        if (targetVersion < VERSION_5_0)
        {
            const auto* variables = reinterpret_cast<const FileConstantBufferVariable*>(shaderByteCode + chunkOffset + fileConstantBuffer.variableOffset);
            if (fileConstantBuffer.variableOffset + sizeof(FileConstantBufferVariable) * fileConstantBuffer.variableCount < chunkSize)
                return false;

            for (auto variableIndex = 0u; variableIndex < fileConstantBuffer.variableCount; variableIndex++)
            {
                const auto& fileVariable = variables[variableIndex];
                ConstantBufferVariable variable;

                if (!PopulateConstantBufferVariable(variable, fileVariable, shaderByteCode, shaderByteCodeSize, chunkOffset, chunkSize))
                    return false;

                constantBuffer.m_variables.emplace_back(std::move(variable));
            }
        }
        else
        {
            const auto* variables = reinterpret_cast<const FileConstantBufferVariable_5_0*>(shaderByteCode + chunkOffset + fileConstantBuffer.variableOffset);
            if (fileConstantBuffer.variableOffset + sizeof(FileConstantBufferVariable_5_0) * fileConstantBuffer.variableCount < chunkSize)
                return false;

            for (auto variableIndex = 0u; variableIndex < fileConstantBuffer.variableCount; variableIndex++)
            {
                const auto& fileVariable = variables[variableIndex];
                ConstantBufferVariable variable;

                if (!PopulateConstantBufferVariable(variable, fileVariable, shaderByteCode, shaderByteCodeSize, chunkOffset, chunkSize))
                    return false;

                constantBuffer.m_variables.emplace_back(std::move(variable));
            }
        }

        return true;
    }

    bool PopulateShaderInfoFromRdef(ShaderInfo& shaderInfo, const uint8_t* shaderByteCode, const size_t shaderByteCodeSize)
    {
        size_t chunkOffset, chunkSize;
        if (!FindChunk(TAG_RDEF, shaderByteCode, shaderByteCodeSize, chunkOffset, chunkSize))
            return false;

        if (sizeof(FileRdefHeader) < chunkSize)
            return false;

        const auto* header = reinterpret_cast<const FileRdefHeader*>(shaderByteCode + chunkOffset);

        const auto targetVersion = header->target & TARGET_VERSION_MASK;
        const auto creatorString = reinterpret_cast<const char*>(shaderByteCode + chunkOffset + header->creatorOffset);

        if (!StringFitsInChunk(creatorString, shaderByteCode, shaderByteCodeSize))
            return false;

        shaderInfo.m_creator = std::string(creatorString);

        if (targetVersion < VERSION_5_1)
        {
            const auto* boundResources = reinterpret_cast<const FileBoundResource*>(shaderByteCode + chunkOffset + header->boundResourceOffset);
            if (header->boundResourceOffset + sizeof(FileBoundResource) * header->boundResourceCount < chunkSize)
                return false;

            for (auto boundResourceIndex = 0u; boundResourceIndex < header->boundResourceCount; boundResourceIndex++)
            {
                const auto& fileBoundResource = boundResources[boundResourceIndex];
                BoundResource boundResource;

                PopulateBoundResource(boundResource, fileBoundResource, shaderByteCode, shaderByteCodeSize, chunkOffset);

                shaderInfo.m_bound_resources.emplace_back(std::move(boundResource));
            }
        }
        else
        {
            const auto* boundResources = reinterpret_cast<const FileBoundResource_5_1*>(shaderByteCode + chunkOffset + header->boundResourceOffset);
            if (header->boundResourceOffset + sizeof(FileBoundResource_5_1) * header->boundResourceCount < chunkSize)
                return false;

            for (auto boundResourceIndex = 0u; boundResourceIndex < header->boundResourceCount; boundResourceIndex++)
            {
                const auto& fileBoundResource = boundResources[boundResourceIndex];
                BoundResource boundResource;

                if (!PopulateBoundResource(boundResource, fileBoundResource, shaderByteCode, shaderByteCodeSize, chunkOffset))
                    return false;

                shaderInfo.m_bound_resources.emplace_back(std::move(boundResource));
            }
        }

        const auto* constantBuffers = reinterpret_cast<const FileConstantBuffer*>(shaderByteCode + chunkOffset + header->constantBufferOffset);
        if (header->constantBufferOffset + sizeof(FileConstantBuffer) * header->constantBufferCount < chunkSize)
            return false;

        for (auto constantBufferIndex = 0u; constantBufferIndex < header->constantBufferCount; constantBufferIndex++)
        {
            const auto& fileConstantBuffer = constantBuffers[constantBufferIndex];
            ConstantBuffer constantBuffer;

            if (!PopulateConstantBuffer(constantBuffer, fileConstantBuffer, shaderByteCode, shaderByteCodeSize, chunkOffset, chunkSize, targetVersion))
                return false;

            shaderInfo.m_constant_buffers.emplace_back(std::move(constantBuffer));
        }

        return true;
    }

    bool PopulateShaderInfoFromShdr(ShaderInfo& shaderInfo, const uint8_t* shaderByteCode, const size_t shaderByteCodeSize)
    {
        size_t chunkOffset, chunkSize;
        if (!FindChunk(TAG_SHDR, shaderByteCode, shaderByteCodeSize, chunkOffset, chunkSize))
            return false;

        if (sizeof(FileShaderHeader) < chunkSize)
            return false;

        const auto* header = reinterpret_cast<const FileShaderHeader*>(shaderByteCode + chunkOffset);

        shaderInfo.m_version_major = header->versionMajor;
        shaderInfo.m_version_minor = header->versionMinor;

        switch (header->programType)
        {
        case D3D10_SB_PIXEL_SHADER:
            shaderInfo.m_type = ShaderType::PIXEL_SHADER;
            break;
        case D3D10_SB_VERTEX_SHADER:
            shaderInfo.m_type = ShaderType::VERTEX_SHADER;
            break;
        case D3D10_SB_GEOMETRY_SHADER:
            shaderInfo.m_type = ShaderType::GEOMETRY_SHADER;
            break;
        case D3D11_SB_HULL_SHADER:
            shaderInfo.m_type = ShaderType::HULL_SHADER;
            break;
        case D3D11_SB_DOMAIN_SHADER:
            shaderInfo.m_type = ShaderType::DOMAIN_SHADER;
            break;
        case D3D11_SB_COMPUTE_SHADER:
            shaderInfo.m_type = ShaderType::COMPUTE_SHADER;
            break;
        default:
            shaderInfo.m_type = ShaderType::UNKNOWN;
            break;
        }

        return true;
    }

    bool PopulateShaderInfoFromBytes(ShaderInfo& shaderInfo, const uint8_t* shaderByteCode, const size_t shaderByteCodeSize)
    {
        if (!PopulateShaderInfoFromRdef(shaderInfo, shaderByteCode, shaderByteCodeSize))
            return false;

        if (!PopulateShaderInfoFromShdr(shaderInfo, shaderByteCode, shaderByteCodeSize))
            return false;

        return true;
    }
} // namespace d3d11

std::unique_ptr<ShaderInfo> ShaderAnalyser::GetShaderInfo(const uint8_t* shader, const size_t shaderSize)
{
    if (shader == nullptr || shaderSize == 0)
        return nullptr;

    auto shaderInfo = std::make_unique<ShaderInfo>();

    if (!PopulateShaderInfoFromBytes(*shaderInfo, shader, shaderSize))
        return nullptr;

    return shaderInfo;
}
