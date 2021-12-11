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

XAssetInfoGeneric* MockAssetLoadingManager::AddAsset(const asset_type_t assetType, const std::string& assetName, void* asset, std::vector<XAssetInfoGeneric*> dependencies,
                                                     std::vector<scr_string_t> usedScriptStrings)
{
	XAssetInfoGeneric assetInfoObj{assetType, assetName, m_zone, std::move(dependencies), std::move(usedScriptStrings), asset};
	auto assetInfo = std::make_unique<XAssetInfoGeneric>(std::move(assetInfoObj));
	const auto assetInfoPtr = assetInfo.get();
	m_added_assets.emplace(std::make_pair(assetInfo->m_name, std::move(assetInfo)));

	return assetInfoPtr;
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

void MockAssetLoadingManager::MockAddAvailableDependency(const asset_type_t assetType, std::string assetName, void* asset)
{
	XAssetInfoGeneric assetInfoObj{assetType, std::move(assetName), m_zone, std::vector<XAssetInfoGeneric*>(), std::vector<scr_string_t>(), asset};
	auto assetInfo = std::make_unique<XAssetInfoGeneric>(std::move(assetInfoObj));
	m_available_dependencies.emplace(std::make_pair(assetInfo->m_name, std::move(assetInfo)));
}

XAssetInfoGeneric* MockAssetLoadingManager::MockGetAddedAsset(const std::string& assetName)
{
	const auto foundAsset = m_added_assets.find(assetName);

	if (foundAsset != m_added_assets.end())
		return foundAsset->second.get();

	return nullptr;
}
