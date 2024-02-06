#include "MockAssetLoadingManager.h"

MockAssetLoadingManager::MockAssetLoadingManager(Zone* zone, ISearchPath* searchPath)
    : m_zone(zone),
      m_mock_gdt(std::make_unique<Gdt>()),
      m_context(std::make_unique<AssetLoadingContext>(zone, searchPath, std::vector({m_mock_gdt.get()})))
{
}

AssetLoadingContext* MockAssetLoadingManager::GetAssetLoadingContext() const
{
    return m_context.get();
}

XAssetInfoGeneric* MockAssetLoadingManager::AddAsset(std::unique_ptr<XAssetInfoGeneric> xAssetInfo)
{
    const auto assetInfoPtr = xAssetInfo.get();
    m_added_assets.emplace(std::make_pair(xAssetInfo->m_name, std::move(xAssetInfo)));

    return assetInfoPtr;
}

XAssetInfoGeneric* MockAssetLoadingManager::AddAsset(const asset_type_t assetType,
                                                     const std::string& assetName,
                                                     void* asset,
                                                     std::vector<XAssetInfoGeneric*> dependencies,
                                                     std::vector<scr_string_t> usedScriptStrings)
{
    return AddAsset(std::make_unique<XAssetInfoGeneric>(assetType, assetName, asset, std::move(dependencies), std::move(usedScriptStrings)));
}

XAssetInfoGeneric* MockAssetLoadingManager::AddAsset(asset_type_t assetType,
                                                     const std::string& assetName,
                                                     void* asset,
                                                     std::vector<XAssetInfoGeneric*> dependencies,
                                                     std::vector<scr_string_t> usedScriptStrings,
                                                     std::vector<IndirectAssetReference> indirectAssetReferences)
{
    return AddAsset(std::make_unique<XAssetInfoGeneric>(
        assetType, assetName, asset, std::move(dependencies), std::move(usedScriptStrings), std::move(indirectAssetReferences)));
}

XAssetInfoGeneric* MockAssetLoadingManager::LoadDependency(const asset_type_t assetType, const std::string& assetName)
{
    auto foundDependencies = m_available_dependencies.find(assetName);

    while (foundDependencies != m_available_dependencies.end())
    {
        if (foundDependencies->second->m_type == assetType)
            return foundDependencies->second.get();

        ++foundDependencies;
    }

    return nullptr;
}

IndirectAssetReference MockAssetLoadingManager::LoadIndirectAssetReference(const asset_type_t assetType, const std::string& assetName)
{
    return IndirectAssetReference(assetType, assetName);
}

void MockAssetLoadingManager::MockAddAvailableDependency(const asset_type_t assetType, std::string assetName, void* asset)
{
    auto assetInfo = std::make_unique<XAssetInfoGeneric>(assetType, std::move(assetName), asset);
    m_available_dependencies.emplace(assetInfo->m_name, std::move(assetInfo));
}

XAssetInfoGeneric* MockAssetLoadingManager::MockGetAddedAsset(const std::string& assetName)
{
    const auto foundAsset = m_added_assets.find(assetName);

    if (foundAsset != m_added_assets.end())
        return foundAsset->second.get();

    return nullptr;
}
