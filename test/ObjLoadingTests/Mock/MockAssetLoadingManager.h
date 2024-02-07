#pragma once

#include "AssetLoading/IAssetLoadingManager.h"

#include <map>
#include <memory>

class MockAssetLoadingManager final : public IAssetLoadingManager
{
    Zone* m_zone;
    std::unique_ptr<Gdt> m_mock_gdt;
    std::unique_ptr<AssetLoadingContext> m_context;
    std::map<std::string, std::unique_ptr<XAssetInfoGeneric>> m_added_assets;
    std::multimap<std::string, std::unique_ptr<XAssetInfoGeneric>> m_available_dependencies;

public:
    MockAssetLoadingManager(Zone* zone, ISearchPath* searchPath);

    _NODISCARD AssetLoadingContext* GetAssetLoadingContext() const override;
    XAssetInfoGeneric* AddAsset(std::unique_ptr<XAssetInfoGeneric> xAssetInfo) override;
    XAssetInfoGeneric* AddAsset(asset_type_t assetType,
                                const std::string& assetName,
                                void* asset,
                                std::vector<XAssetInfoGeneric*> dependencies,
                                std::vector<scr_string_t> usedScriptStrings) override;
    XAssetInfoGeneric* AddAsset(asset_type_t assetType,
                                const std::string& assetName,
                                void* asset,
                                std::vector<XAssetInfoGeneric*> dependencies,
                                std::vector<scr_string_t> usedScriptStrings,
                                std::vector<IndirectAssetReference> indirectAssetReferences) override;
    XAssetInfoGeneric* LoadDependency(asset_type_t assetType, const std::string& assetName) override;
    IndirectAssetReference LoadIndirectAssetReference(asset_type_t assetType, const std::string& assetName) override;

    void MockAddAvailableDependency(asset_type_t assetType, std::string assetName, void* asset);
    XAssetInfoGeneric* MockGetAddedAsset(const std::string& assetName);
};
