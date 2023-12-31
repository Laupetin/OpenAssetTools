#pragma once

#include "AssetLoading/IAssetLoadingManager.h"
#include "Localize/CommonLocalizeEntry.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/Zone.h"

#include <functional>
#include <string>

class LocalizeCommonAssetLoader
{
public:
    explicit LocalizeCommonAssetLoader(std::function<void(const CommonLocalizeEntry&)> entryCallback);

    bool LoadLocalizeAsset(const std::string& assetName, ISearchPath* searchPath, IAssetLoadingManager* manager, Zone* zone) const;

private:
    std::string GetFileName(const std::string& assetName, Zone* zone) const;

    std::function<void(const CommonLocalizeEntry&)> m_entry_callback;
};
