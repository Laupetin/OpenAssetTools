#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
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

    enum class CommonTechniqueShaderType : std::uint8_t
    {
        VERTEX,
        PIXEL
    };

    enum class CommonShaderValueType : std::uint8_t
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

    constexpr bool IsConstValueType(const CommonShaderValueType valueType)
    {
        return valueType == CommonShaderValueType::LITERAL_CONST || valueType == CommonShaderValueType::MATERIAL_CONST
               || valueType == CommonShaderValueType::CODE_CONST;
    }

    constexpr bool IsSamplerValueType(const CommonShaderValueType valueType)
    {
        return valueType == CommonShaderValueType::MATERIAL_SAMPLER || valueType == CommonShaderValueType::CODE_SAMPLER;
    }

    enum class CommonCodeSourceUpdateFrequency : std::uint8_t
    {
        PER_PRIM,
        PER_OBJECT,
        RARELY,
        CUSTOM,
        IGNORE,

        COUNT
    };

    struct CommonCodeConstSourceInfo
    {
        CommonCodeConstSource value;
        const char* accessor;
        std::uint8_t arrayCount;
        CommonCodeSourceUpdateFrequency updateFrequency;
        std::optional<unsigned> techFlags;
        std::optional<CommonCodeConstSource> transposedMatrix;
    };

    struct CommonCodeSamplerSourceInfo
    {
        CommonCodeSamplerSource value;
        const char* accessor;
        CommonCodeSourceUpdateFrequency updateFrequency;
        std::optional<unsigned> techFlags;
        std::optional<unsigned> customSamplerIndex;
    };

    struct CommonShaderArgumentType
    {
        friend bool operator==(const CommonShaderArgumentType& lhs, const CommonShaderArgumentType& rhs)
        {
            return lhs.m_shader_type == rhs.m_shader_type && lhs.m_value_type == rhs.m_value_type;
        }

        friend bool operator!=(const CommonShaderArgumentType& lhs, const CommonShaderArgumentType& rhs)
        {
            return !(lhs == rhs);
        }

        CommonTechniqueShaderType m_shader_type;
        CommonShaderValueType m_value_type;
    };

    class CommonCodeSourceInfos
    {
    public:
        CommonCodeSourceInfos(const CommonCodeConstSourceInfo* codeConstSourceInfos,
                              size_t codeConstCount,
                              const CommonCodeSamplerSourceInfo* codeSamplerSourceInfos,
                              size_t codeSamplerCount,
                              const char** ignoreArgAccessors,
                              size_t ignoredArgAccessorCount,
                              const CommonShaderArgumentType* argumentTypes,
                              size_t argumentTypeCount);

        [[nodiscard]] std::optional<CommonCodeConstSourceInfo> GetInfoForCodeConstSource(CommonCodeConstSource codeConstSource) const;
        [[nodiscard]] std::optional<CommonCodeSamplerSourceInfo> GetInfoForCodeSamplerSource(CommonCodeSamplerSource codeSamplerSource) const;

        /**
         * \brief Some games like T6 do not create args for certain variables. This checks whether an accessor identifies one of these variables.
         * \param accessor The accessor of the variable
         * \return \c true if the accessor should be ignored
         */
        [[nodiscard]] bool IsArgAccessorIgnored(const std::string& accessor) const;

        [[nodiscard]] std::optional<CommonCodeConstSource> GetCodeConstSourceForAccessor(const std::string& accessor) const;
        [[nodiscard]] std::optional<CommonCodeSamplerSource> GetCodeSamplerSourceForAccessor(const std::string& accessor) const;

        [[nodiscard]] std::optional<size_t> GetArgumentTypeNumericValue(const CommonShaderArgumentType& argumentType) const;

    private:
        std::vector<CommonCodeConstSourceInfo> m_code_const_source_infos;
        std::vector<CommonCodeSamplerSourceInfo> m_code_sampler_source_infos;
        std::unordered_set<std::string> m_ignored_arg_accessors;

        std::unordered_map<std::string, CommonCodeConstSource> m_code_const_lookup;
        std::unordered_map<std::string, CommonCodeSamplerSource> m_code_sampler_lookup;

        std::vector<CommonShaderArgumentType> m_argument_types;
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
        [[nodiscard]] std::optional<CommonStreamSource> GetSourceByName(const std::string& name) const;
        [[nodiscard]] std::optional<CommonStreamSource> GetSourceByAbbreviation(const std::string& abbreviation) const;
        [[nodiscard]] std::optional<CommonStreamDestination> GetDestinationByName(const std::string& name) const;
        [[nodiscard]] std::optional<CommonStreamDestination> GetDestinationByAbbreviation(const std::string& abbreviation) const;

    private:
        std::vector<CommonStreamRoutingSourceInfo> m_sources;
        std::vector<CommonStreamRoutingDestinationInfo> m_destinations;
        std::unordered_map<std::string, CommonStreamSource> m_source_name_lookup;
        std::unordered_map<std::string, CommonStreamDestination> m_destination_name_lookup;
        std::unordered_map<std::string, CommonStreamSource> m_source_abbreviation_lookup;
        std::unordered_map<std::string, CommonStreamDestination> m_destination_abbreviation_lookup;
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
        CommonVertexDeclaration() = default;
        explicit CommonVertexDeclaration(std::vector<CommonStreamRouting> routing);

        void SortRoutingEntries();

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
