#include "CommonTechnique.h"

namespace techset
{
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
