#pragma once

#include "Asset/IAssetCreator.h"
#include "Localize/CommonLocalizeEntry.h"
#include "Localize/Parsing/ILocalizeFileDuplicationChecker.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/Zone.h"

#include <string>
#include <unordered_set>

class CommonLocalizeLoader : ILocalizeFileDuplicationChecker
{
public:
    CommonLocalizeLoader(ISearchPath& searchPath, Zone& zone);

    AssetCreationResult CreateLocalizeAsset(const std::string& assetName, AssetCreationContext& context);

protected:
    virtual AssetCreationResult CreateAssetFromCommonAsset(const CommonLocalizeEntry& localizeEntry, AssetCreationContext& context) = 0;

private:
    std::string GetFileName(const std::string& assetName) const;

    bool CheckLocalizeEntryForDuplicates(const std::string& key) override;

    ISearchPath& m_search_path;
    Zone& m_zone;

    std::unordered_set<std::string> m_keys;
};
