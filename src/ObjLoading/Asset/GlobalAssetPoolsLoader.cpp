#include "GlobalAssetPoolsLoader.h"

GlobalAssetPoolsRegistrationPreparation::GlobalAssetPoolsRegistrationPreparation(GenericAssetRegistration& registration,
                                                                                 Zone& zone,
                                                                                 Zone& foreignZone,
                                                                                 AssetCreationContext& context)
    : m_registration(registration),
      m_zone(zone),
      m_foreign_zone(foreignZone),
      m_context(context),
      m_failure(false)
{
}

std::optional<XAssetInfoGeneric*> GlobalAssetPoolsRegistrationPreparation::Visit_Dependency(const asset_type_t assetType, const char* assetName)
{
    if (assetName && assetName[0] == ',')
    {
        /*
        Try to load the actual asset when the asset from the global asset pools just references one.
        If that fails, keep the reference to not destroy previous existing behaviour of just being able to use assets from the global pools
        without ignores.
        */
        auto* nonReferenceAssetName = &assetName[1];

        auto* assetDependency = m_context.LoadDependencyGeneric(assetType, nonReferenceAssetName, false);
        if (assetDependency)
        {
            m_registration.AddDependency(assetDependency);
            return std::nullopt;
        }
    }

    auto* newDependency = m_context.LoadDependencyGeneric(assetType, assetName);
    if (newDependency)
    {
        m_registration.AddDependency(newDependency);
        return std::nullopt;
    }

    m_failure = true;
    return std::nullopt;
}

std::optional<scr_string_t> GlobalAssetPoolsRegistrationPreparation::Visit_ScriptString(scr_string_t scriptString)
{
    // Make sure any used script string is available in the created zone
    m_zone.m_script_strings.AddOrGetScriptString(m_foreign_zone.m_script_strings.CValue(scriptString));
    m_registration.AddScriptString(scriptString);
    return std::nullopt;
}

void GlobalAssetPoolsRegistrationPreparation::Visit_IndirectAssetRef(const asset_type_t assetType, const char* assetName)
{
    m_registration.AddIndirectAssetReference(m_context.LoadIndirectAssetReferenceGeneric(assetType, assetName));
}

bool GlobalAssetPoolsRegistrationPreparation::Failed() const
{
    return m_failure;
}
