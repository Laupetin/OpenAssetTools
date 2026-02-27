#include "D3D11ShaderAnalyser.h"

#include "Utils/FileUtils.h"

#include <cstring>

using namespace d3d11;

namespace d3d11
{
    BoundResource::BoundResource()
        : m_type(BoundResourceType::UNKNOWN),
          m_return_type(BoundResourceReturnType::UNKNOWN),
          m_dimension(BoundResourceDimension::UNKNOWN),
          m_num_samples(0u),
          m_bind_point(0u),
          m_bind_count(0u),
          m_flags(0u)
    {
    }

    ConstantBuffer::ConstantBuffer()
        : m_size(0u),
          m_flags(0u),
          m_type(ConstantBufferType::UNKNOWN)
    {
    }

    ConstantBufferVariable::ConstantBufferVariable()
        : m_offset(0u),
          m_size(0u),
          m_flags(0u),
          m_is_used(false),
          m_variable_class(VariableClass::UNKNOWN),
          m_element_count(0),
          m_row_count(0),
          m_column_count(0)
    {
    }
} // namespace d3d11

namespace
{
    constexpr auto TAG_RDEF = FileUtils::MakeMagic32('R', 'D', 'E', 'F');
    constexpr auto TAG_SHDR = FileUtils::MakeMagic32('S', 'H', 'D', 'R');

    constexpr auto CHUNK_TABLE_OFFSET = 28u;

    struct FileRdefHeader
    {
        uint32_t constantBufferCount;
        uint32_t constantBufferOffset;
        uint32_t boundResourceCount;
        uint32_t boundResourceOffset;
        uint8_t minorVersion;
        uint8_t majorVersion;
        uint16_t type;
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

    // https://learn.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_shader_input_flags
    enum D3D_SHADER_INPUT_FLAGS
    {
        D3D_SIF_USERPACKED = 0x1,
        D3D_SIF_COMPARISON_SAMPLER = 0x2,
        D3D_SIF_TEXTURE_COMPONENT_0 = 0x4,
        D3D_SIF_TEXTURE_COMPONENT_1 = 0x8,
        D3D_SIF_TEXTURE_COMPONENTS = 0xc,
        D3D_SIF_UNUSED = 0x10,
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
        uint32_t uFlags; // D3D_SHADER_INPUT_FLAGS
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

    // https://learn.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_shader_variable_flags
    enum D3D_SHADER_VARIABLE_FLAGS
    {
        D3D_SVF_USERPACKED = 1,
        D3D_SVF_USED = 2,
        D3D_SVF_INTERFACE_POINTER = 4,
        D3D_SVF_INTERFACE_PARAMETER = 8,
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
        uint32_t resourceBinding;
        uint32_t resourceCount;
        uint32_t samplerBinding;
        uint32_t samplerCount;
    };

    static_assert(sizeof(FileConstantBufferVariable) == 24);
    static_assert(sizeof(FileConstantBufferVariable_5_0) == 40);

    enum D3D_SHADER_VARIABLE_CLASS : uint16_t
    {
        D3D_SVC_SCALAR = 0,
        D3D_SVC_VECTOR,
        D3D_SVC_MATRIX_ROWS,
        D3D_SVC_MATRIX_COLUMNS,
        D3D_SVC_OBJECT,
        D3D_SVC_STRUCT,
        D3D_SVC_INTERFACE_CLASS,
        D3D_SVC_INTERFACE_POINTER,
        D3D10_SVC_SCALAR,
        D3D10_SVC_VECTOR,
        D3D10_SVC_MATRIX_ROWS,
        D3D10_SVC_MATRIX_COLUMNS,
        D3D10_SVC_OBJECT,
        D3D10_SVC_STRUCT,
        D3D11_SVC_INTERFACE_CLASS,
        D3D11_SVC_INTERFACE_POINTER
    };

    struct FileRdefType
    {
        D3D_SHADER_VARIABLE_CLASS class_;
        uint16_t baseType;
        uint16_t rowCount;
        uint16_t columnCount;
        uint16_t elementCount;
        uint16_t fieldCount;
        uint32_t fieldsOffset;
    };

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

    constexpr bool IsAtLeastVersion(const uint8_t expectedMajor, const uint8_t expectedMinor, const uint8_t actualMajor, const uint8_t actualMinor)
    {
        if (actualMajor < expectedMajor)
            return false;
        if (actualMinor < expectedMinor)
            return false;

        return true;
    }

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

    VariableClass GetVariableClass(const D3D_SHADER_VARIABLE_CLASS variableClass)
    {
        switch (variableClass)
        {
        case D3D_SVC_SCALAR:
        case D3D10_SVC_SCALAR:
            return VariableClass::SCALAR;
        case D3D_SVC_VECTOR:
        case D3D10_SVC_VECTOR:
            return VariableClass::VECTOR;
        case D3D_SVC_MATRIX_ROWS:
        case D3D10_SVC_MATRIX_ROWS:
            return VariableClass::MATRIX_ROWS;
        case D3D_SVC_MATRIX_COLUMNS:
        case D3D10_SVC_MATRIX_COLUMNS:
            return VariableClass::MATRIX_COLUMNS;
        case D3D_SVC_OBJECT:
        case D3D10_SVC_OBJECT:
            return VariableClass::OBJECT;
        case D3D_SVC_STRUCT:
        case D3D10_SVC_STRUCT:
            return VariableClass::STRUCT;
        case D3D_SVC_INTERFACE_CLASS:
        case D3D11_SVC_INTERFACE_CLASS:
            return VariableClass::INTERFACE_CLASS;
        case D3D_SVC_INTERFACE_POINTER:
        case D3D11_SVC_INTERFACE_POINTER:
            return VariableClass::INTERFACE_POINTER;
        default:
            return VariableClass::UNKNOWN;
        }
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
        constantBufferVariable.m_is_used = fileConstantBufferVariable.flags & D3D_SVF_USED;

        if (fileConstantBufferVariable.typeOffset + sizeof(FileRdefType) > chunkSize)
            return false;

        const auto type = reinterpret_cast<const FileRdefType*>(shaderByteCode + chunkOffset + fileConstantBufferVariable.typeOffset);

        constantBufferVariable.m_variable_class = GetVariableClass(type->class_);
        constantBufferVariable.m_element_count = static_cast<uint16_t>(type->elementCount);
        constantBufferVariable.m_column_count = static_cast<uint16_t>(type->columnCount);
        constantBufferVariable.m_row_count = static_cast<uint16_t>(type->rowCount);

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
                                const uint8_t major,
                                const uint8_t minor)
    {
        const auto nameString = reinterpret_cast<const char*>(shaderByteCode + chunkOffset + fileConstantBuffer.nameOffset);

        if (!StringFitsInChunk(nameString, shaderByteCode, shaderByteCodeSize))
            return false;

        constantBuffer.m_name = std::string(nameString);

        constantBuffer.m_size = fileConstantBuffer.size;
        constantBuffer.m_flags = fileConstantBuffer.flags;
        constantBuffer.m_type = GetType(fileConstantBuffer.type);

        if (IsAtLeastVersion(5, 0, major, minor))
        {
            const auto* variables = reinterpret_cast<const FileConstantBufferVariable_5_0*>(shaderByteCode + chunkOffset + fileConstantBuffer.variableOffset);
            if (fileConstantBuffer.variableOffset + sizeof(FileConstantBufferVariable_5_0) * fileConstantBuffer.variableCount > chunkSize)
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
            const auto* variables = reinterpret_cast<const FileConstantBufferVariable*>(shaderByteCode + chunkOffset + fileConstantBuffer.variableOffset);
            if (fileConstantBuffer.variableOffset + sizeof(FileConstantBufferVariable) * fileConstantBuffer.variableCount > chunkSize)
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

        if (sizeof(FileRdefHeader) > chunkSize)
            return false;

        const auto* header = reinterpret_cast<const FileRdefHeader*>(shaderByteCode + chunkOffset);

        const auto creatorString = reinterpret_cast<const char*>(shaderByteCode + chunkOffset + header->creatorOffset);

        if (!StringFitsInChunk(creatorString, shaderByteCode, shaderByteCodeSize))
            return false;

        shaderInfo.m_creator = std::string(creatorString);

        if (IsAtLeastVersion(5, 1, header->majorVersion, header->minorVersion))
        {
            const auto* boundResources = reinterpret_cast<const FileBoundResource_5_1*>(shaderByteCode + chunkOffset + header->boundResourceOffset);
            if (header->boundResourceOffset + sizeof(FileBoundResource_5_1) * header->boundResourceCount > chunkSize)
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
        else
        {
            const auto* boundResources = reinterpret_cast<const FileBoundResource*>(shaderByteCode + chunkOffset + header->boundResourceOffset);
            if (header->boundResourceOffset + sizeof(FileBoundResource) * header->boundResourceCount > chunkSize)
                return false;

            for (auto boundResourceIndex = 0u; boundResourceIndex < header->boundResourceCount; boundResourceIndex++)
            {
                const auto& fileBoundResource = boundResources[boundResourceIndex];
                BoundResource boundResource;

                PopulateBoundResource(boundResource, fileBoundResource, shaderByteCode, shaderByteCodeSize, chunkOffset);

                shaderInfo.m_bound_resources.emplace_back(std::move(boundResource));
            }
        }

        const auto* constantBuffers = reinterpret_cast<const FileConstantBuffer*>(shaderByteCode + chunkOffset + header->constantBufferOffset);
        if (header->constantBufferOffset + sizeof(FileConstantBuffer) * header->constantBufferCount > chunkSize)
            return false;

        for (auto constantBufferIndex = 0u; constantBufferIndex < header->constantBufferCount; constantBufferIndex++)
        {
            const auto& fileConstantBuffer = constantBuffers[constantBufferIndex];
            ConstantBuffer constantBuffer;

            if (!PopulateConstantBuffer(
                    constantBuffer, fileConstantBuffer, shaderByteCode, shaderByteCodeSize, chunkOffset, chunkSize, header->majorVersion, header->minorVersion))
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

        if (sizeof(FileShaderHeader) > chunkSize)
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
} // namespace

std::unique_ptr<ShaderInfo> ShaderAnalyser::GetShaderInfo(const void* shader, const size_t shaderSize)
{
    if (shader == nullptr || shaderSize == 0)
        return nullptr;

    auto shaderInfo = std::make_unique<ShaderInfo>();

    if (!PopulateShaderInfoFromBytes(*shaderInfo, static_cast<const uint8_t*>(shader), shaderSize))
        return nullptr;

    return std::move(shaderInfo);
}
