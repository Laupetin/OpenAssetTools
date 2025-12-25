#pragma once

#include "Marking/AssetVisitor.h"
#include "Pool/XAssetInfo.h"
#include "Zone/ZoneTypes.h"

#include <optional>
#include <unordered_set>
#include <vector>

class AssetInfoCollector : public AssetVisitor
{
public:
    explicit AssetInfoCollector(Zone& zone);
    ~AssetInfoCollector() override = default;

    [[nodiscard]] std::vector<XAssetInfoGeneric*> GetDependencies() const;
    [[nodiscard]] std::vector<scr_string_t> GetUsedScriptStrings() const;
    [[nodiscard]] std::vector<IndirectAssetReference> GetIndirectAssetReferences() const;

    std::optional<XAssetInfoGeneric*> Visit_Dependency(asset_type_t assetType, const char* assetName) override;
    std::optional<scr_string_t> Visit_ScriptString(scr_string_t scriptString) override;
    void Visit_IndirectAssetRef(asset_type_t assetType, const char* assetName) override;

private:
    std::unordered_set<XAssetInfoGeneric*> m_dependencies;
    std::unordered_set<scr_string_t> m_used_script_strings;
    std::unordered_set<IndirectAssetReference> m_indirect_asset_references;

    Zone& m_zone;
};
