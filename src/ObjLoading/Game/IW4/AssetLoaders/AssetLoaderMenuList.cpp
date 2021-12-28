#include "AssetLoaderMenuList.h"

#include <cstring>
#include <iostream>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/Menu/MenuConverterIW4.h"
#include "Parsing/Menu/MenuFileReader.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderMenuList::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* menuList = memory->Create<MenuList>();
    memset(menuList, 0, sizeof(MenuList));
    menuList->name = memory->Dup(assetName.c_str());
    return menuList;
}

bool AssetLoaderMenuList::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderMenuList::ShouldLoadMenuFile(const std::string& menuFilePath, menu::MenuAssetZoneState* zoneState)
{
    const auto alreadyLoadedFile = zoneState->m_loaded_files.find(menuFilePath);
    if (alreadyLoadedFile == zoneState->m_loaded_files.end())
    {
        zoneState->AddLoadedFile(menuFilePath);
        return true;
    }

    return false;
}

void AssetLoaderMenuList::AddMenuFilesToLoadToQueue(std::deque<std::string>& queue, const menu::ParsingResult* parsingResult, menu::MenuAssetZoneState* zoneState)
{
    for(const auto& menuFileToLoad : parsingResult->m_menus_to_load)
    {
        if(ShouldLoadMenuFile(menuFileToLoad, zoneState))
        {
            queue.push_back(menuFileToLoad);
        }
    }
}

void AssetLoaderMenuList::AddResultsToZoneState(menu::ParsingResult* parsingResult, menu::MenuAssetZoneState* zoneState)
{
    for (auto& function : parsingResult->m_functions)
        zoneState->AddFunction(std::move(function));

    for (auto& menu : parsingResult->m_menus)
        zoneState->AddMenu(std::move(menu));
}

bool AssetLoaderMenuList::ProcessParsedResults(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, menu::ParsingResult* parsingResult,
                                               menu::MenuAssetZoneState* zoneState, std::vector<menuDef_t*>& menus, std::vector<XAssetInfoGeneric*>& menuListDependencies)
{
    const auto menuCount = parsingResult->m_menus.size();
    const auto functionCount = parsingResult->m_functions.size();
    const auto menuLoadCount = parsingResult->m_menus_to_load.size();
    auto totalItemCount = 0u;
    for (const auto& menu : parsingResult->m_menus)
        totalItemCount += menu->m_items.size();

    std::cout << "Successfully read menu file \"" << assetName << "\" (" << menuLoadCount << " loads, " << menuCount << " menus, " << functionCount << " functions, " << totalItemCount << " items)\n";

    for (const auto& menu : parsingResult->m_menus)
    {
        MenuConverter converter(ObjLoading::Configuration.MenuNoOptimization, searchPath, memory, manager);
        auto* menuAsset = converter.ConvertMenu(*menu);
        if(menuAsset == nullptr)
        {
            std::cout << "Failed to convert menu \"" << menu->m_name << "\"\n";
            return false;
        }

        menus.push_back(menuAsset);
        auto* menuAssetInfo = manager->AddAsset(ASSET_TYPE_MENU, menu->m_name, menuAsset, std::move(converter.GetDependencies()), std::vector<scr_string_t>());

        if (menuAssetInfo)
            menuListDependencies.push_back(menuAssetInfo);
    }

    AddResultsToZoneState(parsingResult, zoneState);

    return true;
}

MenuList* AssetLoaderMenuList::CreateMenuListAsset(const std::string& assetName, MemoryManager* memory, const std::vector<menuDef_t*>& menus)
{
    auto* menuListAsset = memory->Create<MenuList>();
    menuListAsset->name = memory->Dup(assetName.c_str());
    menuListAsset->menuCount = static_cast<int>(menus.size());

    if (menuListAsset->menuCount > 0)
    {
        menuListAsset->menus = static_cast<menuDef_t**>(memory->Alloc(sizeof(uintptr_t) * menuListAsset->menuCount));
        for(auto i = 0; i < menuListAsset->menuCount; i++)
            menuListAsset->menus[i] = menus[i];
    }
    else
        menuListAsset->menus = nullptr;

    return menuListAsset;
}

bool AssetLoaderMenuList::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    std::deque<std::string> menuFileQueue;
    std::vector<menuDef_t*> menus;
    std::vector<XAssetInfoGeneric*> menuListDependencies;

    auto* zoneState = manager->GetAssetLoadingContext()->GetZoneAssetLoaderState<menu::MenuAssetZoneState>();
    menuFileQueue.push_back(assetName);

    while (!menuFileQueue.empty())
    {
        const auto& nextMenuFile = menuFileQueue.front();
        const auto file = searchPath->Open(nextMenuFile);
        if (!file.IsOpen())
        {
            std::cout << "Failed to open menu file \"" << nextMenuFile << "\"\n";
            return false;
        }

        menu::MenuFileReader reader(*file.m_stream, nextMenuFile, menu::FeatureLevel::IW4, [searchPath](const std::string& filename, const std::string& sourceFile) -> std::unique_ptr<std::istream>
        {
            auto foundFileToInclude = searchPath->Open(filename);
            if (!foundFileToInclude.IsOpen() || !foundFileToInclude.m_stream)
                return nullptr;

            return std::move(foundFileToInclude.m_stream);
        });

        reader.IncludeZoneState(zoneState);
        reader.SetPermissiveMode(ObjLoading::Configuration.MenuPermissiveParsing);

        const auto menuFileResult = reader.ReadMenuFile();
        if (menuFileResult)
        {
            ProcessParsedResults(nextMenuFile, searchPath, memory, manager, menuFileResult.get(), zoneState, menus, menuListDependencies);
            AddMenuFilesToLoadToQueue(menuFileQueue, menuFileResult.get(), zoneState);
        }
        else
            std::cout << "Could not read menu file \"" << nextMenuFile << "\"\n";

        menuFileQueue.pop_front();
    }

    auto* menuListAsset = CreateMenuListAsset(assetName, memory, menus);

    if(menuListAsset)
        manager->AddAsset(ASSET_TYPE_MENULIST, assetName, menuListAsset);

    return true;
}
