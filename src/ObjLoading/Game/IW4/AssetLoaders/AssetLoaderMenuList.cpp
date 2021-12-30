#include "AssetLoaderMenuList.h"

#include <cstring>
#include <iostream>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/Menu/MenuConversionZoneStateIW4.h"
#include "Game/IW4/Menu/MenuConverterIW4.h"
#include "Parsing/Menu/MenuFileReader.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

namespace IW4
{
    class MenuLoader
    {
    public:
        static bool ProcessParsedResults(const std::string& fileName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, menu::ParsingResult* parsingResult,
            menu::MenuAssetZoneState* zoneState, MenuConversionZoneState* conversionState, std::vector<menuDef_t*>& menus, std::vector<XAssetInfoGeneric*>& menuListDependencies)
        {
            const auto menuCount = parsingResult->m_menus.size();
            const auto functionCount = parsingResult->m_functions.size();
            const auto menuLoadCount = parsingResult->m_menus_to_load.size();
            auto totalItemCount = 0u;
            for (const auto& menu : parsingResult->m_menus)
                totalItemCount += menu->m_items.size();

            std::cout << "Successfully read menu file \"" << fileName << "\" (" << menuLoadCount << " loads, " << menuCount << " menus, " << functionCount << " functions, " << totalItemCount << " items)\n";

            // Add all functions to the zone state to make them available for all menus to be converted
            for (auto& function : parsingResult->m_functions)
                zoneState->AddFunction(std::move(function));

            // Prepare a list of all menus of this file
            std::vector<XAssetInfo<menuDef_t>*> allMenusOfFile;
            allMenusOfFile.reserve(parsingResult->m_menus.size());

            // Convert all menus and add them as assets
            for (auto& menu : parsingResult->m_menus)
            {
                MenuConverter converter(ObjLoading::Configuration.MenuNoOptimization, searchPath, memory, manager);
                auto* menuAsset = converter.ConvertMenu(*menu);
                if (menuAsset == nullptr)
                {
                    std::cout << "Failed to convert menu file \"" << menu->m_name << "\"\n";
                    return false;
                }

                menus.push_back(menuAsset);
                auto* menuAssetInfo = manager->AddAsset(ASSET_TYPE_MENU, menu->m_name, menuAsset, std::move(converter.GetDependencies()), std::vector<scr_string_t>());

                if (menuAssetInfo)
                {
                    allMenusOfFile.push_back(reinterpret_cast<XAssetInfo<menuDef_t>*>(menuAssetInfo));
                    menuListDependencies.push_back(menuAssetInfo);
                }

                zoneState->AddMenu(std::move(menu));
            }

            // Register this file with all loaded menus
            conversionState->AddLoadedFile(fileName, std::move(allMenusOfFile));

            return true;
        }

        static MenuList* CreateMenuListAsset(const std::string& assetName, MemoryManager* memory, const std::vector<menuDef_t*>& menus)
        {
            auto* menuListAsset = memory->Create<MenuList>();
            menuListAsset->name = memory->Dup(assetName.c_str());
            menuListAsset->menuCount = static_cast<int>(menus.size());

            if (menuListAsset->menuCount > 0)
            {
                menuListAsset->menus = static_cast<menuDef_t**>(memory->Alloc(sizeof(uintptr_t) * menuListAsset->menuCount));
                for (auto i = 0; i < menuListAsset->menuCount; i++)
                    menuListAsset->menus[i] = menus[i];
            }
            else
                menuListAsset->menus = nullptr;

            return menuListAsset;
        }

        static std::unique_ptr<menu::ParsingResult> ParseMenuFile(const std::string& menuFileName, ISearchPath* searchPath, const menu::MenuAssetZoneState* zoneState)
        {
            const auto file = searchPath->Open(menuFileName);
            if (!file.IsOpen())
            {
                std::cerr << "Failed to open menu file \"" << menuFileName << "\"\n";
                return nullptr;
            }

            menu::MenuFileReader reader(*file.m_stream, menuFileName, menu::FeatureLevel::IW4, [searchPath](const std::string& filename, const std::string& sourceFile) -> std::unique_ptr<std::istream>
            {
                auto foundFileToInclude = searchPath->Open(filename);
                if (!foundFileToInclude.IsOpen() || !foundFileToInclude.m_stream)
                    return nullptr;

                return std::move(foundFileToInclude.m_stream);
            });

            reader.IncludeZoneState(zoneState);
            reader.SetPermissiveMode(ObjLoading::Configuration.MenuPermissiveParsing);

            return reader.ReadMenuFile();
        }
    };
}

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

bool AssetLoaderMenuList::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    std::vector<menuDef_t*> menus;
    std::vector<XAssetInfoGeneric*> menuListDependencies;

    auto* zoneState = manager->GetAssetLoadingContext()->GetZoneAssetLoaderState<menu::MenuAssetZoneState>();
    auto* conversionState = manager->GetAssetLoadingContext()->GetZoneAssetLoaderState<MenuConversionZoneState>();

    const auto menuListResult = MenuLoader::ParseMenuFile(assetName, searchPath, zoneState);
    if (menuListResult)
        MenuLoader::ProcessParsedResults(assetName, searchPath, memory, manager, menuListResult.get(), zoneState, conversionState, menus, menuListDependencies);
    else
        std::cerr << "Could not read menu list file \"" << assetName << "\"\n";

    for(const auto& menuFileToLoad : menuListResult->m_menus_to_load)
    {
        const auto alreadyLoadedMenuFile = conversionState->m_menus_by_filename.find(menuFileToLoad);
        if(alreadyLoadedMenuFile != conversionState->m_menus_by_filename.end())
        {
            std::cout << "Already loaded \"" << menuFileToLoad << "\", skipping\n";
            for (auto* menu : alreadyLoadedMenuFile->second)
            {
                menus.push_back(menu->Asset());
                menuListDependencies.push_back(menu);
            }
            continue;
        }

        const auto menuFileResult = MenuLoader::ParseMenuFile(menuFileToLoad, searchPath, zoneState);
        if (menuFileResult)
        {
            MenuLoader::ProcessParsedResults(menuFileToLoad, searchPath, memory, manager, menuFileResult.get(), zoneState, conversionState, menus, menuListDependencies);
            if (!menuFileResult->m_menus_to_load.empty())
                std::cout << "WARNING: Menu file has menus to load even though it is not a menu list, ignoring: \"" << menuFileToLoad << "\"\n";
        }
        else
            std::cerr << "Could not read menu file \"" << menuFileToLoad << "\"\n";
    }

    auto* menuListAsset = MenuLoader::CreateMenuListAsset(assetName, memory, menus);

    if (menuListAsset)
        manager->AddAsset(ASSET_TYPE_MENULIST, assetName, menuListAsset, menuListDependencies, std::vector<scr_string_t>());

    return true;
}

void AssetLoaderMenuList::FinalizeAssetsForZone(AssetLoadingContext* context) const
{
    context->GetZoneAssetLoaderState<MenuConversionZoneState>()->FinalizeSupportingData();
}
