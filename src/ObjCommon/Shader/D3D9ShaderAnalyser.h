#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace d3d9
{
    enum class ShaderType : std::uint8_t
    {
        UNKNOWN,
        PIXEL_SHADER,
        VERTEX_SHADER
    };

    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxregister-set
    enum class RegisterSet : std::uint8_t
    {
        BOOL,
        INT_4,
        FLOAT_4,
        SAMPLER,

        // This entry only exist to mark the size of the enum and is not an actual valid value
        MAX
    };

    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxparameter-class
    enum class ParameterClass : std::uint8_t
    {
        SCALAR,
        VECTOR,
        MATRIX_ROWS,
        MATRIX_COLUMNS,
        OBJECT,
        STRUCT,

        // This entry only exist to mark the size of the enum and is not an actual valid value
        MAX
    };

    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxparameter-type
    enum class ParameterType : std::uint8_t
    {
        VOID,
        BOOL,
        INT,
        FLOAT,
        STRING,
        TEXTURE,
        TEXTURE_1D,
        TEXTURE_2D,
        TEXTURE_3D,
        TEXTURE_CUBE,
        SAMPLER,
        SAMPLER_1D,
        SAMPLER_2D,
        SAMPLER_3D,
        SAMPLER_CUBE,
        PIXEL_SHADER,
        VERTEX_SHADER,
        PIXEL_FRAGMENT,
        VERTEX_FRAGMENT,
        UNSUPPORTED,

        // This entry only exist to mark the size of the enum and is not an actual valid value
        MAX
    };

    class ShaderConstant
    {
    public:
        std::string m_name;
        RegisterSet m_register_set{};
        unsigned m_register_index = 0;
        unsigned m_register_count = 0;
        ParameterClass m_class{};
        ParameterType m_type{};
        unsigned m_type_rows = 0;
        unsigned m_type_columns = 0;
        unsigned m_type_elements = 0;
    };

    class ShaderInfo
    {
    public:
        ShaderType m_type = ShaderType::UNKNOWN;
        unsigned m_version_major = 0;
        unsigned m_version_minor = 0;
        std::string m_creator;
        std::string m_target;
        std::vector<ShaderConstant> m_constants;
    };

    class ShaderAnalyser
    {
    public:
        static std::unique_ptr<ShaderInfo> GetShaderInfo(const void* shaderByteCode, size_t shaderByteCodeSize);
    };
} // namespace d3d9
