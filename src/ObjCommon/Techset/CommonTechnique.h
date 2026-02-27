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

    struct CommonShaderArgCodeConstValue
    {
        CommonCodeConstSource m_index;
        unsigned m_first_row;
        unsigned m_row_count;
    };

    union CommonShaderArgValue
    {
        std::array<float, 4> literal_value;
        CommonShaderArgCodeConstValue code_const_source;
        CommonCodeSamplerSource code_sampler_source;
        unsigned name_hash;
    };

    class CommonShaderArgDestinationDx9
    {
    public:
        unsigned m_destination_register;
    };

    // In case of a constant: Offset in constant buffer
    // In case of a sampler: Index of sampler + index of texture
    union CommonShaderArgLocationDx11
    {
        unsigned constant_buffer_offset;

        struct
        {
            unsigned texture_index;
            unsigned sampler_index;
        };
    };

    class CommonShaderArgDestinationDx11
    {
    public:
        CommonShaderArgLocationDx11 m_location;
        unsigned m_size;
        unsigned m_buffer;
    };

    union CommonShaderArgDestination
    {
        CommonShaderArgDestinationDx9 dx9;
        CommonShaderArgDestinationDx11 dx11;
    };

    class CommonShaderArg
    {
    public:
        CommonShaderArg() = default;
        CommonShaderArg(CommonShaderArgumentType type, const CommonShaderArgDestination& destination, const CommonShaderArgValue& value);

        [[nodiscard]] CommonCodeSourceUpdateFrequency GetFrequency(const CommonCodeSourceInfos& infos) const;

        CommonShaderArgumentType m_type;
        CommonShaderArgDestination m_destination;
        CommonShaderArgValue m_value;
        std::optional<CommonCodeSourceUpdateFrequency> m_bin;
    };

    class CommonStreamRouting
    {
    public:
        CommonStreamRouting() = default;
        CommonStreamRouting(CommonStreamSource source, CommonStreamDestination destination);

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

    class CommonTechniqueShaderBin
    {
    public:
        const void* m_shader_bin;
        size_t m_shader_bin_size;
    };

    class CommonTechniqueShader
    {
    public:
        CommonTechniqueShader();
        CommonTechniqueShader(CommonTechniqueShaderType type, std::string name);

        CommonTechniqueShaderType m_type;
        std::string m_name;
        std::optional<CommonTechniqueShaderBin> m_bin;
    };

    class CommonPass
    {
    public:
        using FrequencyCounts_t = std::array<size_t, std::to_underlying(CommonCodeSourceUpdateFrequency::COUNT)>;

        CommonPass() = default;
        CommonPass(uint32_t samplerFlags,
                   std::string stateMap,
                   CommonTechniqueShader vertexShader,
                   CommonTechniqueShader pixelShader,
                   CommonVertexDeclaration vertexDeclaration);

        [[nodiscard]] FrequencyCounts_t GetFrequencyCounts(const CommonCodeSourceInfos& infos) const;

        uint32_t m_sampler_flags;
        std::string m_state_map;
        CommonTechniqueShader m_vertex_shader;
        CommonTechniqueShader m_pixel_shader;
        CommonVertexDeclaration m_vertex_declaration;
        std::vector<CommonShaderArg> m_args;
    };

    class CommonTechnique
    {
    public:
        CommonTechnique() = default;
        explicit CommonTechnique(std::string name);
        CommonTechnique(std::string name, uint64_t flags);

        std::string m_name;
        uint64_t m_flags;
        std::vector<CommonPass> m_passes;
    };

    enum class DxVersion : std::uint8_t
    {
        DX9,
        DX11
    };
} // namespace techset
