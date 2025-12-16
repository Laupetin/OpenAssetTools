#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace techset
{
    typedef std::uint8_t CommonStreamSource;
    typedef std::uint8_t CommonStreamDestination;

    typedef std::uint8_t CommonCodeConstSource;
    typedef std::uint8_t CommonCodeSamplerSource;

    struct CommonStreamRoutingSourceInfo
    {
        const char* name;
        const char* abbreviation;
        bool optional;
    };

    struct CommonStreamRoutingDestinationInfo
    {
        const char* name;
        const char* abbreviation;
    };

    enum class CommonCodeSourceUpdateFrequency : std::uint8_t
    {
        PER_PRIM,
        PER_OBJECT,
        RARELY,
        CUSTOM,
    };

    struct CommonCodeConstSourceInfo
    {
        CommonCodeConstSource value;
        const char* accessor;
        std::uint8_t arrayCount;
        CommonCodeSourceUpdateFrequency updateFrequency;
    };

    struct CommonCodeSamplerSourceInfo
    {
        CommonCodeSamplerSource value;
        const char* accessor;
        CommonCodeSourceUpdateFrequency updateFrequency;
    };

    class CommonCodeSourceInfos
    {
    public:
        CommonCodeSourceInfos(const CommonCodeConstSourceInfo* codeConstSourceInfos,
                              size_t codeConstCount,
                              const CommonCodeSamplerSourceInfo* codeSamplerSourceInfos,
                              size_t codeSamplerCount);

        [[nodiscard]] std::optional<CommonCodeConstSourceInfo> GetInfoForCodeConstSource(CommonCodeConstSource codeConstSource) const;
        [[nodiscard]] std::optional<CommonCodeSamplerSourceInfo> GetInfoForCodeSamplerSource(CommonCodeSamplerSource codeSamplerSource) const;

    private:
        std::vector<CommonCodeConstSourceInfo> m_code_const_source_infos;
        std::vector<CommonCodeSamplerSourceInfo> m_code_sampler_source_infos;
    };

    class CommonStreamRoutingInfos
    {
    public:
        CommonStreamRoutingInfos(const CommonStreamRoutingSourceInfo* sourceInfos,
                                 size_t sourceCount,
                                 const CommonStreamRoutingDestinationInfo* destinationNames,
                                 size_t destinationCount);

        [[nodiscard]] const char* GetSourceName(CommonStreamSource source) const;
        [[nodiscard]] const char* GetSourceAbbreviation(CommonStreamSource source) const;
        [[nodiscard]] bool IsSourceOptional(CommonStreamSource source) const;
        [[nodiscard]] const char* GetDestinationName(CommonStreamDestination destination) const;
        [[nodiscard]] const char* GetDestinationAbbreviation(CommonStreamDestination destination) const;

    private:
        std::vector<CommonStreamRoutingSourceInfo> m_sources;
        std::vector<CommonStreamRoutingDestinationInfo> m_destinations;
    };

    union CommonShaderArgValue
    {
        std::array<float, 4> literal_value;
        CommonCodeConstSource code_const_source;
        CommonCodeSamplerSource code_sampler_source;
        unsigned name_hash;
    };

    class CommonShaderArgDestinationDx9
    {
    public:
        unsigned m_destination_register;
    };

    class CommonShaderArgDestinationDx11
    {
    public:
        // In case of a constant: Offset in constant buffer
        // In case of a sampler: Index of sampler
        unsigned m_location;
        unsigned m_size;
        unsigned m_buffer;
    };

    union CommonShaderArgDestination
    {
        CommonShaderArgDestinationDx9 dx9;
        CommonShaderArgDestinationDx11 dx11;
    };

    enum class CommonShaderArgType : std::uint8_t
    {
        // Value is set to a float4 value in the pass
        LITERAL_CONST,
        // Value is set to a float4 value in the material
        MATERIAL_CONST,
        // Value is set to a float4 value calculated in code
        CODE_CONST,
        // Value is set to a sampler from the material
        MATERIAL_SAMPLER,
        // Value is set to a sampler generated in code
        CODE_SAMPLER
    };

    class CommonShaderArg
    {
    public:
        CommonShaderArgType m_type;
        CommonShaderArgDestination m_destination;
        CommonShaderArgValue m_value;
    };

    class CommonStreamRouting
    {
    public:
        CommonStreamSource m_source;
        CommonStreamDestination m_destination;
    };

    class CommonVertexDeclaration
    {
    public:
        std::vector<CommonStreamRouting> m_routing;
    };

    class CommonTechniqueShader
    {
    public:
        std::string m_name;
        const void* m_shader_bin;
        size_t m_shader_bin_size;
        std::vector<CommonShaderArg> m_args;
    };

    enum class DxVersion : std::uint8_t
    {
        DX9,
        DX11
    };

    class CommonPass
    {
    public:
        uint32_t m_sampler_flags;
        DxVersion m_dx_version;
        CommonTechniqueShader m_vertex_shader;
        CommonTechniqueShader m_pixel_shader;
        CommonVertexDeclaration m_vertex_declaration;
    };

    class CommonTechnique
    {
    public:
        std::string m_name;
        uint64_t m_flags;
        std::vector<CommonPass> m_passes;
    };
} // namespace techset
