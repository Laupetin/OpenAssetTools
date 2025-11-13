#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace d3d11
{
    enum class ShaderType : std::uint8_t
    {
        UNKNOWN,
        PIXEL_SHADER,
        VERTEX_SHADER,
        GEOMETRY_SHADER,
        HULL_SHADER,
        DOMAIN_SHADER,
        COMPUTE_SHADER
    };

    class ConstantBufferVariable
    {
    public:
        ConstantBufferVariable();
        ~ConstantBufferVariable() = default;
        ConstantBufferVariable(const ConstantBufferVariable& other) = default;
        ConstantBufferVariable(ConstantBufferVariable&& other) noexcept = default;
        ConstantBufferVariable& operator=(const ConstantBufferVariable& other) = default;
        ConstantBufferVariable& operator=(ConstantBufferVariable&& other) noexcept = default;

        std::string m_name;
        unsigned m_offset;
        unsigned m_size;
        unsigned m_flags;
    };

    enum class ConstantBufferType : std::uint8_t
    {
        UNKNOWN,
        CBUFFER,
        TBUFFER,
        INTERFACE_POINTERS,
        RESOURCE_BIND_INFO,
    };

    class ConstantBuffer
    {
    public:
        ConstantBuffer();
        ~ConstantBuffer() = default;
        ConstantBuffer(const ConstantBuffer& other) = default;
        ConstantBuffer(ConstantBuffer&& other) noexcept = default;
        ConstantBuffer& operator=(const ConstantBuffer& other) = default;
        ConstantBuffer& operator=(ConstantBuffer&& other) noexcept = default;

        std::string m_name;
        unsigned m_size;
        unsigned m_flags;
        ConstantBufferType m_type;
        std::vector<ConstantBufferVariable> m_variables;
    };

    enum class BoundResourceType : std::uint8_t
    {
        UNKNOWN,
        CBUFFER,
        TBUFFER,
        TEXTURE,
        SAMPLER
    };

    enum class BoundResourceReturnType : std::uint8_t
    {
        UNKNOWN,
        UNORM,
        SNORM,
        SINT,
        UINT,
        FLOAT,
        MIXED,
        DOUBLE,
        CONTINUED,
    };

    enum class BoundResourceDimension : std::uint8_t
    {
        UNKNOWN,
        BUFFER,
        TEXTURE_1D,
        TEXTURE_1D_ARRAY,
        TEXTURE_2D,
        TEXTURE_2D_ARRAY,
        TEXTURE_2D_MS,
        TEXTURE_2D_MS_ARRAY,
        TEXTURE_3D,
        TEXTURE_CUBE,
        TEXTURE_CUBE_ARRAY,
        BUFFER_EX,
    };

    class BoundResource
    {
    public:
        BoundResource();
        ~BoundResource() = default;
        BoundResource(const BoundResource& other) = default;
        BoundResource(BoundResource&& other) noexcept = default;
        BoundResource& operator=(const BoundResource& other) = default;
        BoundResource& operator=(BoundResource&& other) noexcept = default;

        std::string m_name;
        BoundResourceType m_type;
        BoundResourceReturnType m_return_type;
        BoundResourceDimension m_dimension;
        unsigned m_num_samples;
        unsigned m_bind_point;
        unsigned m_bind_count;
        unsigned m_flags;
    };

    class ShaderInfo
    {
    public:
        ShaderType m_type = ShaderType::UNKNOWN;
        unsigned m_version_major = 0;
        unsigned m_version_minor = 0;
        std::string m_creator;
        std::vector<ConstantBuffer> m_constant_buffers;
        std::vector<BoundResource> m_bound_resources;
    };

    class ShaderAnalyser
    {
    public:
        static std::unique_ptr<ShaderInfo> GetShaderInfo(const void* shader, size_t shaderSize);
    };
} // namespace d3d11
