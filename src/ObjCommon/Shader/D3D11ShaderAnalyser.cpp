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

    // https://learn.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_shader_input_type
    enum D3D_SHADER_INPUT_TYPE : uint32_t
    {
        D3D_SIT_CBUFFER = 0,
        D3D_SIT_TBUFFER,
        D3D_SIT_TEXTURE,
        D3D_SIT_SAMPLER,
        D3D_SIT_UAV_RWTYPED,
        D3D_SIT_STRUCTURED,
        D3D_SIT_UAV_RWSTRUCTURED,
        D3D_SIT_BYTEADDRESS,
        D3D_SIT_UAV_RWBYTEADDRESS,
        D3D_SIT_UAV_APPEND_STRUCTURED,
        D3D_SIT_UAV_CONSUME_STRUCTURED,
        D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER,
        D3D_SIT_RTACCELERATIONSTRUCTURE,
        D3D_SIT_UAV_FEEDBACKTEXTURE,
        D3D10_SIT_CBUFFER,
        D3D10_SIT_TBUFFER,
        D3D10_SIT_TEXTURE,
        D3D10_SIT_SAMPLER,
        D3D11_SIT_UAV_RWTYPED,
        D3D11_SIT_STRUCTURED,
        D3D11_SIT_UAV_RWSTRUCTURED,
        D3D11_SIT_BYTEADDRESS,
        D3D11_SIT_UAV_RWBYTEADDRESS,
        D3D11_SIT_UAV_APPEND_STRUCTURED,
        D3D11_SIT_UAV_CONSUME_STRUCTURED,
        D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER
    };

    // https://learn.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_resource_return_type
    enum D3D_RESOURCE_RETURN_TYPE : uint32_t
    {
        D3D_RETURN_TYPE_UNORM = 1,
        D3D_RETURN_TYPE_SNORM = 2,
        D3D_RETURN_TYPE_SINT = 3,
        D3D_RETURN_TYPE_UINT = 4,
        D3D_RETURN_TYPE_FLOAT = 5,
        D3D_RETURN_TYPE_MIXED = 6,
        D3D_RETURN_TYPE_DOUBLE = 7,
        D3D_RETURN_TYPE_CONTINUED = 8,
        D3D10_RETURN_TYPE_UNORM,
        D3D10_RETURN_TYPE_SNORM,
        D3D10_RETURN_TYPE_SINT,
        D3D10_RETURN_TYPE_UINT,
        D3D10_RETURN_TYPE_FLOAT,
        D3D10_RETURN_TYPE_MIXED,
        D3D11_RETURN_TYPE_UNORM,
        D3D11_RETURN_TYPE_SNORM,
        D3D11_RETURN_TYPE_SINT,
        D3D11_RETURN_TYPE_UINT,
        D3D11_RETURN_TYPE_FLOAT,
        D3D11_RETURN_TYPE_MIXED,
        D3D11_RETURN_TYPE_DOUBLE,
        D3D11_RETURN_TYPE_CONTINUED
    };

    // https://learn.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_srv_dimension
    enum D3D_SRV_DIMENSION : uint32_t
    {
        D3D_SRV_DIMENSION_UNKNOWN = 0,
        D3D_SRV_DIMENSION_BUFFER = 1,
        D3D_SRV_DIMENSION_TEXTURE1D = 2,
        D3D_SRV_DIMENSION_TEXTURE1DARRAY = 3,
        D3D_SRV_DIMENSION_TEXTURE2D = 4,
        D3D_SRV_DIMENSION_TEXTURE2DARRAY = 5,
        D3D_SRV_DIMENSION_TEXTURE2DMS = 6,
        D3D_SRV_DIMENSION_TEXTURE2DMSARRAY = 7,
        D3D_SRV_DIMENSION_TEXTURE3D = 8,
        D3D_SRV_DIMENSION_TEXTURECUBE = 9,
        D3D_SRV_DIMENSION_TEXTURECUBEARRAY = 10,
        D3D_SRV_DIMENSION_BUFFEREX = 11,
        D3D10_SRV_DIMENSION_UNKNOWN,
        D3D10_SRV_DIMENSION_BUFFER,
        D3D10_SRV_DIMENSION_TEXTURE1D,
        D3D10_SRV_DIMENSION_TEXTURE1DARRAY,
        D3D10_SRV_DIMENSION_TEXTURE2D,
        D3D10_SRV_DIMENSION_TEXTURE2DARRAY,
        D3D10_SRV_DIMENSION_TEXTURE2DMS,
        D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY,
        D3D10_SRV_DIMENSION_TEXTURE3D,
        D3D10_SRV_DIMENSION_TEXTURECUBE,
        D3D10_1_SRV_DIMENSION_UNKNOWN,
        D3D10_1_SRV_DIMENSION_BUFFER,
        D3D10_1_SRV_DIMENSION_TEXTURE1D,
        D3D10_1_SRV_DIMENSION_TEXTURE1DARRAY,
        D3D10_1_SRV_DIMENSION_TEXTURE2D,
        D3D10_1_SRV_DIMENSION_TEXTURE2DARRAY,
        D3D10_1_SRV_DIMENSION_TEXTURE2DMS,
        D3D10_1_SRV_DIMENSION_TEXTURE2DMSARRAY,
        D3D10_1_SRV_DIMENSION_TEXTURE3D,
        D3D10_1_SRV_DIMENSION_TEXTURECUBE,
        D3D10_1_SRV_DIMENSION_TEXTURECUBEARRAY,
        D3D11_SRV_DIMENSION_UNKNOWN,
        D3D11_SRV_DIMENSION_BUFFER,
        D3D11_SRV_DIMENSION_TEXTURE1D,
        D3D11_SRV_DIMENSION_TEXTURE1DARRAY,
        D3D11_SRV_DIMENSION_TEXTURE2D,
        D3D11_SRV_DIMENSION_TEXTURE2DARRAY,
        D3D11_SRV_DIMENSION_TEXTURE2DMS,
        D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY,
        D3D11_SRV_DIMENSION_TEXTURE3D,
        D3D11_SRV_DIMENSION_TEXTURECUBE,
        D3D11_SRV_DIMENSION_TEXTURECUBEARRAY,
        D3D11_SRV_DIMENSION_BUFFEREX
    };

    struct FileBoundResource
    {
        uint32_t nameOffset;
        D3D_SHADER_INPUT_TYPE type;
        D3D_RESOURCE_RETURN_TYPE returnType;
        D3D_SRV_DIMENSION dimension;
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

    // https://learn.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_cbuffer_type
    enum D3D_CBUFFER_TYPE : uint32_t
    {
        D3D_CT_CBUFFER = 0,
        D3D_CT_TBUFFER,
        D3D_CT_INTERFACE_POINTERS,
        D3D_CT_RESOURCE_BIND_INFO,
        D3D10_CT_CBUFFER,
        D3D10_CT_TBUFFER,
        D3D11_CT_CBUFFER,
        D3D11_CT_TBUFFER,
        D3D11_CT_INTERFACE_POINTERS,
        D3D11_CT_RESOURCE_BIND_INFO
    };

    struct FileConstantBuffer
    {
        uint32_t nameOffset;
        uint32_t variableCount;
        uint32_t variableOffset;
        uint32_t size;
        uint32_t flags;
        D3D_CBUFFER_TYPE type;
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

    BoundResourceType GetType(const D3D_SHADER_INPUT_TYPE type)
    {
        switch (type)
        {
        case D3D_SIT_CBUFFER:
        case D3D10_SIT_CBUFFER:
            return BoundResourceType::CBUFFER;
        case D3D_SIT_TBUFFER:
        case D3D10_SIT_TBUFFER:
            return BoundResourceType::TBUFFER;
        case D3D_SIT_TEXTURE:
        case D3D10_SIT_TEXTURE:
            return BoundResourceType::TEXTURE;
        case D3D_SIT_SAMPLER:
        case D3D10_SIT_SAMPLER:
            return BoundResourceType::SAMPLER;
        default:
            return BoundResourceType::UNKNOWN;
        }
    }

    BoundResourceReturnType GetReturnType(const D3D_RESOURCE_RETURN_TYPE returnType)
    {
        switch (returnType)
        {
        case D3D_RETURN_TYPE_UNORM:
        case D3D10_RETURN_TYPE_UNORM:
        case D3D11_RETURN_TYPE_UNORM:
            return BoundResourceReturnType::UNORM;
        case D3D_RETURN_TYPE_SNORM:
        case D3D10_RETURN_TYPE_SNORM:
        case D3D11_RETURN_TYPE_SNORM:
            return BoundResourceReturnType::SNORM;
        case D3D_RETURN_TYPE_SINT:
        case D3D10_RETURN_TYPE_SINT:
        case D3D11_RETURN_TYPE_SINT:
            return BoundResourceReturnType::SINT;
        case D3D_RETURN_TYPE_UINT:
        case D3D10_RETURN_TYPE_UINT:
        case D3D11_RETURN_TYPE_UINT:
            return BoundResourceReturnType::UINT;
        case D3D_RETURN_TYPE_FLOAT:
        case D3D10_RETURN_TYPE_FLOAT:
        case D3D11_RETURN_TYPE_FLOAT:
            return BoundResourceReturnType::FLOAT;
        case D3D_RETURN_TYPE_MIXED:
        case D3D10_RETURN_TYPE_MIXED:
        case D3D11_RETURN_TYPE_MIXED:
            return BoundResourceReturnType::MIXED;
        case D3D_RETURN_TYPE_DOUBLE:
        case D3D11_RETURN_TYPE_DOUBLE:
            return BoundResourceReturnType::DOUBLE;
        case D3D_RETURN_TYPE_CONTINUED:
        case D3D11_RETURN_TYPE_CONTINUED:
            return BoundResourceReturnType::CONTINUED;
        default:
            return BoundResourceReturnType::UNKNOWN;
        }
    }

    BoundResourceDimension GetDimension(const D3D_SRV_DIMENSION dimension)
    {
        switch (dimension)
        {
        case D3D_SRV_DIMENSION_BUFFER:
        case D3D10_SRV_DIMENSION_BUFFER:
        case D3D10_1_SRV_DIMENSION_BUFFER:
        case D3D11_SRV_DIMENSION_BUFFER:
            return BoundResourceDimension::BUFFER;
        case D3D_SRV_DIMENSION_TEXTURE1D:
        case D3D10_SRV_DIMENSION_TEXTURE1D:
        case D3D10_1_SRV_DIMENSION_TEXTURE1D:
        case D3D11_SRV_DIMENSION_TEXTURE1D:
            return BoundResourceDimension::TEXTURE_1D;
        case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
        case D3D10_SRV_DIMENSION_TEXTURE1DARRAY:
        case D3D10_1_SRV_DIMENSION_TEXTURE1DARRAY:
        case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
            return BoundResourceDimension::TEXTURE_1D_ARRAY;
        case D3D_SRV_DIMENSION_TEXTURE2D:
        case D3D10_SRV_DIMENSION_TEXTURE2D:
        case D3D10_1_SRV_DIMENSION_TEXTURE2D:
        case D3D11_SRV_DIMENSION_TEXTURE2D:
            return BoundResourceDimension::TEXTURE_2D;
        case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
        case D3D10_SRV_DIMENSION_TEXTURE2DARRAY:
        case D3D10_1_SRV_DIMENSION_TEXTURE2DARRAY:
        case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
            return BoundResourceDimension::TEXTURE_2D_ARRAY;
        case D3D_SRV_DIMENSION_TEXTURE2DMS:
        case D3D10_SRV_DIMENSION_TEXTURE2DMS:
        case D3D10_1_SRV_DIMENSION_TEXTURE2DMS:
        case D3D11_SRV_DIMENSION_TEXTURE2DMS:
            return BoundResourceDimension::TEXTURE_2D_MS;
        case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
        case D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY:
        case D3D10_1_SRV_DIMENSION_TEXTURE2DMSARRAY:
        case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
            return BoundResourceDimension::TEXTURE_2D_MS_ARRAY;
        case D3D_SRV_DIMENSION_TEXTURE3D:
        case D3D10_SRV_DIMENSION_TEXTURE3D:
        case D3D10_1_SRV_DIMENSION_TEXTURE3D:
        case D3D11_SRV_DIMENSION_TEXTURE3D:
            return BoundResourceDimension::TEXTURE_3D;
        case D3D_SRV_DIMENSION_TEXTURECUBE:
        case D3D10_SRV_DIMENSION_TEXTURECUBE:
        case D3D10_1_SRV_DIMENSION_TEXTURECUBE:
        case D3D11_SRV_DIMENSION_TEXTURECUBE:
            return BoundResourceDimension::TEXTURE_CUBE;
        case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:
        case D3D10_1_SRV_DIMENSION_TEXTURECUBEARRAY:
        case D3D11_SRV_DIMENSION_TEXTURECUBEARRAY:
            return BoundResourceDimension::TEXTURE_CUBE_ARRAY;
        case D3D_SRV_DIMENSION_BUFFEREX:
        case D3D11_SRV_DIMENSION_BUFFEREX:
            return BoundResourceDimension::BUFFER_EX;
        default:
            return BoundResourceDimension::UNKNOWN;
        }
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
        boundResource.m_type = GetType(fileBoundResource.type);
        boundResource.m_return_type = GetReturnType(fileBoundResource.returnType);
        boundResource.m_dimension = GetDimension(fileBoundResource.dimension);
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

    ConstantBufferType GetType(const D3D_CBUFFER_TYPE type)
    {
        switch (type)
        {
        case D3D_CT_CBUFFER:
        case D3D10_CT_CBUFFER:
        case D3D11_CT_CBUFFER:
            return ConstantBufferType::CBUFFER;
        case D3D_CT_TBUFFER:
        case D3D10_CT_TBUFFER:
        case D3D11_CT_TBUFFER:
            return ConstantBufferType::TBUFFER;
        case D3D_CT_INTERFACE_POINTERS:
        case D3D11_CT_INTERFACE_POINTERS:
            return ConstantBufferType::INTERFACE_POINTERS;
        case D3D_CT_RESOURCE_BIND_INFO:
        case D3D11_CT_RESOURCE_BIND_INFO:
            return ConstantBufferType::RESOURCE_BIND_INFO;
        default:
            return ConstantBufferType::UNKNOWN;
        }
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
        constantBuffer.m_type = GetType(fileConstantBuffer.type);

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

    ShaderType GetShaderType(const FileProgramType programType)
    {
        switch (programType)
        {
        case D3D10_SB_PIXEL_SHADER:
            return ShaderType::PIXEL_SHADER;
        case D3D10_SB_VERTEX_SHADER:
            return ShaderType::VERTEX_SHADER;
        case D3D10_SB_GEOMETRY_SHADER:
            return ShaderType::GEOMETRY_SHADER;
        case D3D11_SB_HULL_SHADER:
            return ShaderType::HULL_SHADER;
        case D3D11_SB_DOMAIN_SHADER:
            return ShaderType::DOMAIN_SHADER;
        case D3D11_SB_COMPUTE_SHADER:
            return ShaderType::COMPUTE_SHADER;
        default:
            return ShaderType::UNKNOWN;
        }
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

        shaderInfo.m_type = GetShaderType(header->programType);

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
