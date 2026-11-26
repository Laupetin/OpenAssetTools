#include "BrandingAssetCreator.h"

#include "GitVersion.h"

#include <format>

namespace
{
    constexpr auto MOD_NAME_PROPERTY = "mod.name";
    constexpr auto MOD_AUTHOR_PROPERTY = "mod.author";

    const std::string* GetProperty(const ZoneDefinition& zoneDefinition, const char* propertyName)
    {
        const auto property = zoneDefinition.m_properties.m_properties.find(propertyName);
        if (property == zoneDefinition.m_properties.m_properties.end() || property->second.empty())
            return nullptr;

        return &property->second;
    }
} // namespace

namespace raw_file
{
    std::string CreateBrandingText(const ZoneDefinition& zoneDefinition, const std::string_view targetName, const std::string_view version)
    {
        const auto* configuredModName = GetProperty(zoneDefinition, MOD_NAME_PROPERTY);
        const auto modName = configuredModName ? std::string_view(*configuredModName) : targetName;

        auto brandingText = std::format("generator.name=OpenAssetTools\ngenerator.version={}\nmod.name={}", version, modName);

        const auto* modAuthor = GetProperty(zoneDefinition, MOD_AUTHOR_PROPERTY);
        if (modAuthor)
            brandingText.append(std::format("\nmod.author={}", *modAuthor));

        return brandingText;
    }

    std::string CreateBrandingText(const ZoneDefinition& zoneDefinition, const std::string_view targetName)
    {
        return CreateBrandingText(zoneDefinition, targetName, GIT_VERSION);
    }
} // namespace raw_file
