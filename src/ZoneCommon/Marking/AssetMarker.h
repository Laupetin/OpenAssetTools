#pragma once

#include "Pool/XAssetInfo.h"
#include "Zone/ZoneTypes.h"

#include <unordered_set>

class AssetMarker
{
public:
    [[nodiscard]] std::vector<XAssetInfoGeneric*> GetDependencies() const;
    [[nodiscard]] std::vector<scr_string_t> GetUsedScriptStrings() const;
    [[nodiscard]] std::vector<IndirectAssetReference> GetIndirectAssetReferences() const;

protected:
    AssetMarker(asset_type_t assetType, Zone& zone);

    void AddDependency(XAssetInfoGeneric* assetInfo);

    void Mark_ScriptString(scr_string_t scrString);
    void MarkArray_ScriptString(const scr_string_t* scrStringArray, size_t count);

    void Mark_IndirectAssetRef(asset_type_t type, const char* assetRefName);
    void MarkArray_IndirectAssetRef(asset_type_t type, const char** assetRefNames, size_t count);

    [[nodiscard]] XAssetInfoGeneric* GetAssetInfoByName(const std::string& name) const;

    Zone& m_zone;

private:
    asset_type_t m_asset_type;

    std::unordered_set<XAssetInfoGeneric*> m_dependencies;
    std::unordered_set<scr_string_t> m_used_script_strings;
    std::unordered_set<IndirectAssetReference> m_indirect_asset_references;
};
