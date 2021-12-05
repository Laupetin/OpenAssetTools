#pragma once

#include <deque>

#include "Game/IW4/IW4.h"
#include "AssetLoading/BasicAssetLoader.h"
#include "AssetLoading/IAssetLoadingManager.h"
#include "Parsing/Menu/MenuAssetZoneState.h"
#include "Parsing/Menu/Domain/MenuParsingResult.h"
#include "SearchPath/ISearchPath.h"

namespace IW4
{
    class AssetLoaderMenuList final : public BasicAssetLoader<ASSET_TYPE_MENULIST, MenuList>
    {
        static bool ShouldLoadMenuFile(const std::string& menuFilePath, menu::MenuAssetZoneState* zoneState);
        static void AddMenuFilesToLoadToQueue(std::deque<std::string>& queue, const menu::ParsingResult* parsingResult, menu::MenuAssetZoneState* zoneState);
        static void AddResultsToZoneState(menu::ParsingResult* parsingResult, menu::MenuAssetZoneState* zoneState);
        static bool ProcessParsedResults(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, menu::ParsingResult* parsingResult, menu::MenuAssetZoneState* zoneState, std::vector<menuDef_t*>& menus, std::vector<XAssetInfoGeneric*>& menuListDependencies);
        static MenuList* CreateMenuListAsset(const std::string& assetName, MemoryManager* memory, const std::vector<menuDef_t*>& menus);

    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        bool LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
}
