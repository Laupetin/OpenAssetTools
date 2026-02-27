#include "CommonTechnique.h"

#include <algorithm>
#include <cassert>

namespace techset
{
    CommonCodeSourceInfos::CommonCodeSourceInfos(const CommonCodeConstSourceInfo* codeConstSourceInfos,
                                                 const size_t codeConstCount,
                                                 const CommonCodeSamplerSourceInfo* codeSamplerSourceInfos,
                                                 const size_t codeSamplerCount,
                                                 const char** ignoreArgAccessors,
                                                 const size_t ignoredArgAccessorCount,
                                                 const CommonShaderArgumentType* argumentTypes,
                                                 const size_t argumentTypeCount)
        : m_code_const_source_infos(codeConstCount),
          m_code_sampler_source_infos(codeSamplerCount),
          m_ignored_arg_accessors(ignoredArgAccessorCount),
          m_argument_types(argumentTypeCount)
    {
        std::copy(codeConstSourceInfos, &codeConstSourceInfos[codeConstCount], m_code_const_source_infos.data());
        std::ranges::sort(m_code_const_source_infos,
                          [](const CommonCodeConstSourceInfo& a, const CommonCodeConstSourceInfo& b) -> bool
                          {
                              return a.value < b.value;
                          });

        std::copy(codeSamplerSourceInfos, &codeSamplerSourceInfos[codeSamplerCount], m_code_sampler_source_infos.data());
        std::ranges::sort(m_code_sampler_source_infos,
                          [](const CommonCodeSamplerSourceInfo& a, const CommonCodeSamplerSourceInfo& b) -> bool
                          {
                              return a.value < b.value;
                          });

        for (size_t i = 0; i < ignoredArgAccessorCount; i++)
            m_ignored_arg_accessors.emplace(ignoreArgAccessors[i]);

        std::copy(argumentTypes, &argumentTypes[argumentTypeCount], m_argument_types.data());

        for (const auto& codeConstInfo : m_code_const_source_infos)
        {
            m_code_const_lookup.emplace(codeConstInfo.accessor, codeConstInfo.value);
        }

        for (const auto& codeSamplerInfo : m_code_sampler_source_infos)
        {
            m_code_sampler_lookup.emplace(codeSamplerInfo.accessor, codeSamplerInfo.value);
        }
    }

    std::optional<CommonCodeConstSourceInfo> CommonCodeSourceInfos::GetInfoForCodeConstSource(const CommonCodeConstSource codeConstSource) const
    {
        for (const auto& codeConstSourceInfo : m_code_const_source_infos)
        {
            const auto codeConstSourceInfoEnd = static_cast<unsigned>(codeConstSourceInfo.value) + codeConstSourceInfo.arrayCount;
            if (codeConstSourceInfo.value <= codeConstSource && codeConstSourceInfoEnd >= codeConstSource)
                return codeConstSourceInfo;

            if (codeConstSourceInfoEnd > codeConstSource)
                return std::nullopt;
        }

        return std::nullopt;
    }

    std::optional<CommonCodeSamplerSourceInfo> CommonCodeSourceInfos::GetInfoForCodeSamplerSource(const CommonCodeSamplerSource codeSamplerSource) const
    {
        for (const auto& codeSamplerSourceInfo : m_code_sampler_source_infos)
        {
            if (codeSamplerSourceInfo.value == codeSamplerSource)
                return codeSamplerSourceInfo;

            if (codeSamplerSourceInfo.value > codeSamplerSource)
                return std::nullopt;
        }

        return std::nullopt;
    }

    bool CommonCodeSourceInfos::IsArgAccessorIgnored(const std::string& accessor) const
    {
        return m_ignored_arg_accessors.contains(accessor);
    }

    std::optional<CommonCodeConstSource> CommonCodeSourceInfos::GetCodeConstSourceForAccessor(const std::string& accessor) const
    {
        const auto foundEntry = m_code_const_lookup.find(accessor);
        if (foundEntry == m_code_const_lookup.end())
            return std::nullopt;

        return foundEntry->second;
    }

    std::optional<CommonCodeSamplerSource> CommonCodeSourceInfos::GetCodeSamplerSourceForAccessor(const std::string& accessor) const
    {
        const auto foundEntry = m_code_sampler_lookup.find(accessor);
        if (foundEntry == m_code_sampler_lookup.end())
            return std::nullopt;

        return foundEntry->second;
    }

    std::optional<size_t> CommonCodeSourceInfos::GetArgumentTypeNumericValue(const CommonShaderArgumentType& argumentType) const
    {
        const auto foundValue = std::ranges::find(m_argument_types, argumentType);
        if (foundValue == m_argument_types.end())
            return std::nullopt;

        return static_cast<size_t>(foundValue - m_argument_types.begin());
    }

    CommonStreamRoutingInfos::CommonStreamRoutingInfos(const CommonStreamRoutingSourceInfo* sourceInfos,
                                                       const size_t sourceCount,
                                                       const CommonStreamRoutingDestinationInfo* destinationNames,
                                                       const size_t destinationCount)
        : m_sources(sourceCount),
          m_destinations(destinationCount)
    {
        std::copy(sourceInfos, &sourceInfos[sourceCount], m_sources.data());
        std::copy(destinationNames, &destinationNames[destinationCount], m_destinations.data());

        for (size_t i = 0; i < sourceCount; i++)
        {
            m_source_name_lookup[sourceInfos[i].name] = static_cast<CommonStreamSource>(i);
            m_source_abbreviation_lookup[sourceInfos[i].abbreviation] = static_cast<CommonStreamSource>(i);
        }

        for (size_t i = 0; i < destinationCount; i++)
        {
            m_destination_name_lookup[destinationNames[i].name] = static_cast<CommonStreamDestination>(i);
            m_destination_abbreviation_lookup[destinationNames[i].abbreviation] = static_cast<CommonStreamDestination>(i);
        }
    }

    const char* CommonStreamRoutingInfos::GetSourceName(const CommonStreamSource source) const
    {
        if (source >= m_sources.size())
            return nullptr;

        return m_sources[source].name;
    }

    const char* CommonStreamRoutingInfos::GetSourceAbbreviation(const CommonStreamSource source) const
    {
        if (source >= m_sources.size())
            return nullptr;

        return m_sources[source].abbreviation;
    }

    bool CommonStreamRoutingInfos::IsSourceOptional(const CommonStreamSource source) const
    {
        if (source >= m_sources.size())
            return false;

        return m_sources[source].optional;
    }

    const char* CommonStreamRoutingInfos::GetDestinationName(const CommonStreamDestination destination) const
    {
        if (destination >= m_destinations.size())
            return nullptr;

        return m_destinations[destination].name;
    }

    const char* CommonStreamRoutingInfos::GetDestinationAbbreviation(const CommonStreamDestination destination) const
    {
        if (destination >= m_destinations.size())
            return nullptr;

        return m_destinations[destination].abbreviation;
    }

    std::optional<CommonStreamSource> CommonStreamRoutingInfos::GetSourceByName(const std::string& name) const
    {
        const auto foundSource = m_source_name_lookup.find(name);
        if (foundSource != m_source_name_lookup.end())
            return foundSource->second;

        return std::nullopt;
    }

    std::optional<CommonStreamSource> CommonStreamRoutingInfos::GetSourceByAbbreviation(const std::string& abbreviation) const
    {
        const auto foundSource = m_source_abbreviation_lookup.find(abbreviation);
        if (foundSource != m_source_abbreviation_lookup.end())
            return foundSource->second;

        return std::nullopt;
    }

    std::optional<CommonStreamDestination> CommonStreamRoutingInfos::GetDestinationByName(const std::string& name) const
    {
        const auto foundDestination = m_destination_name_lookup.find(name);
        if (foundDestination != m_destination_name_lookup.end())
            return foundDestination->second;

        return std::nullopt;
    }

    std::optional<CommonStreamDestination> CommonStreamRoutingInfos::GetDestinationByAbbreviation(const std::string& abbreviation) const
    {
        const auto foundDestination = m_destination_abbreviation_lookup.find(abbreviation);
        if (foundDestination != m_destination_abbreviation_lookup.end())
            return foundDestination->second;

        return std::nullopt;
    }

    CommonShaderArg::CommonShaderArg(const CommonShaderArgumentType type, const CommonShaderArgDestination& destination, const CommonShaderArgValue& value)
        : m_type(type),
          m_destination(destination),
          m_value(value)
    {
    }

    CommonCodeSourceUpdateFrequency CommonShaderArg::GetFrequency(const CommonCodeSourceInfos& infos) const
    {
        switch (m_type.m_value_type)
        {
        case CommonShaderValueType::CODE_CONST:
        {
            const auto info = infos.GetInfoForCodeConstSource(m_value.code_const_source.m_index);
            assert(info);
            return info->updateFrequency;
        }

        case CommonShaderValueType::CODE_SAMPLER:
        {
            const auto info = infos.GetInfoForCodeSamplerSource(m_value.code_sampler_source);
            assert(info);
            return info->updateFrequency;
        }

        case CommonShaderValueType::MATERIAL_CONST:
        case CommonShaderValueType::LITERAL_CONST:
        case CommonShaderValueType::MATERIAL_SAMPLER:
            return CommonCodeSourceUpdateFrequency::RARELY;

        default:
            assert(false);
            return CommonCodeSourceUpdateFrequency::RARELY;
        }
    }

    CommonStreamRouting::CommonStreamRouting(const CommonStreamSource source, const CommonStreamDestination destination)
        : m_source(source),
          m_destination(destination)
    {
    }

    CommonVertexDeclaration::CommonVertexDeclaration(std::vector<CommonStreamRouting> routing)
        : m_routing(std::move(routing))
    {
    }

    void CommonVertexDeclaration::SortRoutingEntries()
    {
        std::ranges::sort(m_routing,
                          [](const CommonStreamRouting& r1, const CommonStreamRouting& r2)
                          {
                              return r1.m_source < r2.m_source;
                          });
    }

    CommonTechniqueShader::CommonTechniqueShader()
        : m_type(CommonTechniqueShaderType::VERTEX)
    {
    }

    CommonTechniqueShader::CommonTechniqueShader(const CommonTechniqueShaderType type, std::string name)
        : m_type(type),
          m_name(std::move(name))
    {
    }

    CommonPass::CommonPass(const uint32_t samplerFlags,
                           std::string stateMap,
                           CommonTechniqueShader vertexShader,
                           CommonTechniqueShader pixelShader,
                           CommonVertexDeclaration vertexDeclaration)
        : m_sampler_flags(samplerFlags),
          m_state_map(std::move(stateMap)),
          m_vertex_shader(std::move(vertexShader)),
          m_pixel_shader(std::move(pixelShader)),
          m_vertex_declaration(std::move(vertexDeclaration))
    {
    }

    CommonPass::FrequencyCounts_t CommonPass::GetFrequencyCounts(const CommonCodeSourceInfos& infos) const
    {
        FrequencyCounts_t result;
        for (auto& count : result)
            count = 0;

        for (auto& arg : m_args)
            result[std::to_underlying(arg.GetFrequency(infos))]++;

        return result;
    }

    CommonTechnique::CommonTechnique(std::string name)
        : m_name(std::move(name)),
          m_flags(0)
    {
    }

    CommonTechnique::CommonTechnique(std::string name, const uint64_t flags)
        : m_name(std::move(name)),
          m_flags(flags)
    {
    }
} // namespace techset
