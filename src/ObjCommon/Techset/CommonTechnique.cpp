#include "CommonTechnique.h"

#include <algorithm>

namespace techset
{
    CommonCodeSourceInfos::CommonCodeSourceInfos(const CommonCodeConstSourceInfo* codeConstSourceInfos,
                                                 const size_t codeConstCount,
                                                 const CommonCodeSamplerSourceInfo* codeSamplerSourceInfos,
                                                 const size_t codeSamplerCount)
        : m_code_const_source_infos(codeConstCount),
          m_code_sampler_source_infos(codeSamplerCount)
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

    CommonStreamRoutingInfos::CommonStreamRoutingInfos(const CommonStreamRoutingSourceInfo* sourceInfos,
                                                       const size_t sourceCount,
                                                       const CommonStreamRoutingDestinationInfo* destinationNames,
                                                       const size_t destinationCount)
        : m_sources(sourceCount),
          m_destinations(destinationCount)
    {
        std::copy(sourceInfos, &sourceInfos[sourceCount], m_sources.data());
        std::copy(destinationNames, &destinationNames[destinationCount], m_destinations.data());
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
} // namespace techset
