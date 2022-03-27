#pragma once

#include <memory>
#include <string>
#include <vector>

namespace techset
{
    enum class ShaderArgumentType
    {
        CODE_CONSTANT,
        LITERAL_CONSTANT,
        MATERIAL_CONSTANT
    };

    class ShaderArgumentLiteralConstant
    {
    public:
        float m_value[4];

        ShaderArgumentLiteralConstant();
        ShaderArgumentLiteralConstant(float v0, float v1, float v2, float v3);
        explicit ShaderArgumentLiteralConstant(float value[4]);

        friend bool operator<(const ShaderArgumentLiteralConstant& lhs, const ShaderArgumentLiteralConstant& rhs);
        friend bool operator<=(const ShaderArgumentLiteralConstant& lhs, const ShaderArgumentLiteralConstant& rhs);
        friend bool operator>(const ShaderArgumentLiteralConstant& lhs, const ShaderArgumentLiteralConstant& rhs);
        friend bool operator>=(const ShaderArgumentLiteralConstant& lhs, const ShaderArgumentLiteralConstant& rhs);
    };

    class ShaderArgumentDefinition
    {
    public:
        ShaderArgumentType m_type;
        std::string m_shader_argument_name;
        size_t m_shader_argument_index;

        std::vector<std::string> m_code_constant_accessors;

        ShaderArgumentLiteralConstant m_literal_constant;

        bool m_material_constant_is_hash;
        size_t m_material_constant_hash;
        std::string m_material_constant_name;
    };

    class ShaderDefinition
    {
    public:
        size_t m_version_major;
        size_t m_version_minor;
        std::string m_shader_name;
        std::vector<ShaderArgumentDefinition> m_arguments;
    };

    class VertexStreamRoutingDefinition
    {
        std::string m_destination_name;
        std::string m_source_name;
    };

    class TechniqueDefinition
    {
    public:
        std::string m_state_map;
        std::unique_ptr<ShaderDefinition> m_vertex_shader;
        std::unique_ptr<ShaderDefinition> m_pixel_shader;
        std::vector<VertexStreamRoutingDefinition> m_vertex_stream_routing;
    };
}
