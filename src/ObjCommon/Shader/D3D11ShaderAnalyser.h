#pragma once

#include <memory>
#include <string>
#include <vector>

namespace d3d11
{
    enum class ShaderType
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
        unsigned m_type;
        std::vector<ConstantBufferVariable> m_variables;
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
        unsigned m_type;
        unsigned m_return_type;
        unsigned m_dimension;
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
        static std::unique_ptr<ShaderInfo> GetShaderInfo(const uint8_t* shader, size_t shaderSize);
    };
} // namespace d3d11
