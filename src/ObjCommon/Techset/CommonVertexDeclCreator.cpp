#include "CommonVertexDeclCreator.h"

#include "Utils/Logging/Log.h"

#include <string>

namespace
{
    bool NextAbbreviation(const std::string& assetName, std::string& abbreviation, size_t& offset)
    {
        if (offset >= assetName.size())
            return false;

        auto digitCount = 0;
        while (offset + digitCount + 1 < assetName.size() && isdigit(assetName[offset + digitCount + 1]))
            digitCount++;

        abbreviation = std::string(assetName, offset, digitCount + 1);
        offset += digitCount + 1;

        return true;
    }
} // namespace

namespace techset
{
    std::optional<CommonVertexDeclaration> CreateVertexDeclFromName(const std::string& name, const CommonStreamRoutingInfos& routingInfos)
    {
        CommonVertexDeclaration result;
        size_t currentOffset = 0u;

        std::string sourceAbbreviation;
        while (NextAbbreviation(name, sourceAbbreviation, currentOffset))
        {
            std::string destinationAbbreviation;
            if (!NextAbbreviation(name, destinationAbbreviation, currentOffset))
            {
                con::error("Failed to detect vertex decl destination abbreviation: {}", name);
                return std::nullopt;
            }

            const auto maybeSource = routingInfos.GetSourceByAbbreviation(sourceAbbreviation);
            if (!maybeSource)
            {
                con::error("Unknown vertex decl source abbreviation: {}", sourceAbbreviation);
                return std::nullopt;
            }

            const auto maybeDestination = routingInfos.GetDestinationByAbbreviation(destinationAbbreviation);
            if (!maybeDestination)
            {
                con::error("Unknown vertex decl destination abbreviation: {}", destinationAbbreviation);
                return std::nullopt;
            }

            result.m_routing.emplace_back(*maybeSource, *maybeDestination);
        }

        result.SortRoutingEntries();

        return result;
    }
} // namespace techset
