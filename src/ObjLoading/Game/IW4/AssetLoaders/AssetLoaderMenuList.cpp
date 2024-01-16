#include "AssetLoaderMenuList.h"

#include "Game/IW4/Cache/MenuCacheReader.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/Menu/MenuConversionZoneStateIW4.h"
#include "Game/IW4/Menu/MenuConverterIW4.h"
#include "ObjLoading.h"
#include "Parsing/Menu/MenuFileReader.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <iostream>

using namespace IW4;

namespace IW4
{
    void EnrichMenu(ISearchPath* searchPath, menuDef_t* menu, MemoryManager* memory)
    {
        std::ostringstream ss;
        ss << "menubin/" << menu->window.name << ".bin";
        const auto file = searchPath->Open(ss.str());

        if (!file.IsOpen())
            return;

        const MenuCacheReader reader(*file.m_stream, *memory, menu->expressionData);
        const auto readMenu = reader.ReadMenu();

        // const auto* globalAsset = GlobalAssetPool<menuDef_t>::GetAssetByName(menu->window.name);
        // const auto* readMenu = globalAsset->Asset();

#define COPY(p, a, b) (a)->p = (b)->p
#define MEMCOPY(p, a, b) memcpy(&a->p, &b->p, sizeof(a->p))

        COPY(window.name, menu, readMenu);
        COPY(window.rect, menu, readMenu);
        COPY(window.rectClient, menu, readMenu);
        COPY(window.group, menu, readMenu);
        COPY(window.style, menu, readMenu);
        COPY(window.border, menu, readMenu);
        COPY(window.ownerDraw, menu, readMenu);
        COPY(window.ownerDrawFlags, menu, readMenu);
        COPY(window.borderSize, menu, readMenu);
        COPY(window.staticFlags, menu, readMenu);
        COPY(window.dynamicFlags[0], menu, readMenu);
        COPY(window.nextTime, menu, readMenu);
        MEMCOPY(window.foreColor, menu, readMenu);
        MEMCOPY(window.backColor, menu, readMenu);
        MEMCOPY(window.borderColor, menu, readMenu);
        MEMCOPY(window.outlineColor, menu, readMenu);
        MEMCOPY(window.disableColor, menu, readMenu);
        COPY(font, menu, readMenu);
        COPY(fullScreen, menu, readMenu);
        COPY(fontIndex, menu, readMenu);
        COPY(cursorItem[0], menu, readMenu);
        COPY(fadeCycle, menu, readMenu);
        COPY(fadeClamp, menu, readMenu);
        COPY(fadeAmount, menu, readMenu);
        COPY(fadeInAmount, menu, readMenu);
        COPY(blurRadius, menu, readMenu);
        COPY(onOpen, menu, readMenu);
        COPY(onClose, menu, readMenu);
        COPY(onCloseRequest, menu, readMenu);
        COPY(onESC, menu, readMenu);
        COPY(onKey, menu, readMenu);
        COPY(visibleExp, menu, readMenu);
        COPY(allowedBinding, menu, readMenu);
        COPY(soundName, menu, readMenu);
        COPY(imageTrack, menu, readMenu);
        MEMCOPY(focusColor, menu, readMenu);
        COPY(rectXExp, menu, readMenu);
        COPY(rectYExp, menu, readMenu);
        COPY(rectWExp, menu, readMenu);
        COPY(rectHExp, menu, readMenu);
        COPY(openSoundExp, menu, readMenu);
        COPY(closeSoundExp, menu, readMenu);
        MEMCOPY(scaleTransition, menu, readMenu);
        MEMCOPY(alphaTransition, menu, readMenu);
        MEMCOPY(xTransition, menu, readMenu);
        MEMCOPY(yTransition, menu, readMenu);

        const auto itemCount = std::min(menu->itemCount, readMenu->itemCount);
        for (auto i = 0; i < itemCount; i++)
        {
            auto* item = menu->items[i];
            const auto* readItem = readMenu->items[i];

            COPY(window.name, item, readItem);
            COPY(window.rect, item, readItem);
            COPY(window.rectClient, item, readItem);
            COPY(window.group, item, readItem);
            COPY(window.style, item, readItem);
            COPY(window.border, item, readItem);
            COPY(window.ownerDraw, item, readItem);
            COPY(window.ownerDrawFlags, item, readItem);
            COPY(window.borderSize, item, readItem);
            COPY(window.staticFlags, item, readItem);
            COPY(window.dynamicFlags[0], item, readItem);
            COPY(window.nextTime, item, readItem);
            MEMCOPY(window.foreColor, item, readItem);
            MEMCOPY(window.backColor, item, readItem);
            MEMCOPY(window.borderColor, item, readItem);
            MEMCOPY(window.outlineColor, item, readItem);
            MEMCOPY(window.disableColor, item, readItem);
            MEMCOPY(textRect[0], item, readItem);
            COPY(type, item, readItem);
            COPY(dataType, item, readItem);
            COPY(alignment, item, readItem);
            COPY(fontEnum, item, readItem);
            COPY(textAlignMode, item, readItem);
            COPY(textalignx, item, readItem);
            COPY(textaligny, item, readItem);
            COPY(textscale, item, readItem);
            COPY(textStyle, item, readItem);
            COPY(gameMsgWindowIndex, item, readItem);
            COPY(gameMsgWindowMode, item, readItem);
            COPY(text, item, readItem);
            COPY(itemFlags, item, readItem);
            COPY(mouseEnterText, item, readItem);
            COPY(mouseExitText, item, readItem);
            COPY(mouseEnter, item, readItem);
            COPY(mouseExit, item, readItem);
            COPY(action, item, readItem);
            COPY(accept, item, readItem);
            COPY(onFocus, item, readItem);
            COPY(leaveFocus, item, readItem);
            COPY(dvar, item, readItem);
            COPY(dvarTest, item, readItem);
            COPY(onKey, item, readItem);
            COPY(enableDvar, item, readItem);
            COPY(localVar, item, readItem);
            COPY(dvarFlags, item, readItem);
            COPY(special, item, readItem);
            COPY(cursorPos[0], item, readItem);
            COPY(imageTrack, item, readItem);
            COPY(visibleExp, item, readItem);
            COPY(disabledExp, item, readItem);
            COPY(textExp, item, readItem);
            COPY(materialExp, item, readItem);
            MEMCOPY(glowColor, item, readItem);
            COPY(decayActive, item, readItem);
            COPY(fxBirthTime, item, readItem);
            COPY(fxLetterTime, item, readItem);
            COPY(fxDecayStartTime, item, readItem);
            COPY(fxDecayDuration, item, readItem);
            COPY(lastSoundPlayedTime, item, readItem);
        }

        std::cout << "Enriched \"" << menu->window.name << "\"!\n";
    }

    class MenuLoader
    {
    public:
        static bool ProcessParsedResults(const std::string& fileName,
                                         ISearchPath* searchPath,
                                         MemoryManager* memory,
                                         IAssetLoadingManager* manager,
                                         menu::ParsingResult* parsingResult,
                                         menu::MenuAssetZoneState* zoneState,
                                         MenuConversionZoneState* conversionState,
                                         std::vector<menuDef_t*>& menus,
                                         std::vector<XAssetInfoGeneric*>& menuListDependencies)
        {
            const auto menuCount = parsingResult->m_menus.size();
            const auto functionCount = parsingResult->m_functions.size();
            const auto menuLoadCount = parsingResult->m_menus_to_load.size();
            auto totalItemCount = 0u;
            for (const auto& menu : parsingResult->m_menus)
                totalItemCount += menu->m_items.size();

            std::cout << "Successfully read menu file \"" << fileName << "\" (" << menuLoadCount << " loads, " << menuCount << " menus, " << functionCount
                      << " functions, " << totalItemCount << " items)\n";

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

                EnrichMenu(searchPath, menuAsset, memory);

                menus.push_back(menuAsset);
                auto* menuAssetInfo =
                    manager->AddAsset(ASSET_TYPE_MENU, menu->m_name, menuAsset, std::move(converter.GetDependencies()), std::vector<scr_string_t>());

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

        static std::unique_ptr<menu::ParsingResult>
            ParseMenuFile(const std::string& menuFileName, ISearchPath* searchPath, const menu::MenuAssetZoneState* zoneState)
        {
            const auto file = searchPath->Open(menuFileName);
            if (!file.IsOpen())
                return nullptr;

            menu::MenuFileReader reader(*file.m_stream,
                                        menuFileName,
                                        menu::FeatureLevel::IW4,
                                        [searchPath](const std::string& filename, const std::string& sourceFile) -> std::unique_ptr<std::istream>
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
} // namespace IW4

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

bool BuildMenuFileQueue(std::deque<std::string>& menuLoadQueue,
                        const std::string& menuListAssetName,
                        ISearchPath* searchPath,
                        MemoryManager* memory,
                        IAssetLoadingManager* manager,
                        menu::MenuAssetZoneState* zoneState,
                        MenuConversionZoneState* conversionState,
                        std::vector<menuDef_t*>& menus,
                        std::vector<XAssetInfoGeneric*>& menuListDependencies)
{
    const auto alreadyLoadedMenuListFileMenus = conversionState->m_menus_by_filename.find(menuListAssetName);

    if (alreadyLoadedMenuListFileMenus == conversionState->m_menus_by_filename.end())
    {
        const auto menuListResult = MenuLoader::ParseMenuFile(menuListAssetName, searchPath, zoneState);
        if (menuListResult)
        {
            MenuLoader::ProcessParsedResults(
                menuListAssetName, searchPath, memory, manager, menuListResult.get(), zoneState, conversionState, menus, menuListDependencies);

            for (const auto& menuToLoad : menuListResult->m_menus_to_load)
                menuLoadQueue.push_back(menuToLoad);

            zoneState->AddMenusToLoad(menuListAssetName, std::move(menuListResult->m_menus_to_load));
        }
        else
            return false;
    }

    return true;
}

void LoadMenuFileFromQueue(const std::string& menuFilePath,
                           ISearchPath* searchPath,
                           MemoryManager* memory,
                           IAssetLoadingManager* manager,
                           menu::MenuAssetZoneState* zoneState,
                           MenuConversionZoneState* conversionState,
                           std::vector<menuDef_t*>& menus,
                           std::vector<XAssetInfoGeneric*>& menuListDependencies)
{
    const auto alreadyLoadedMenuFile = conversionState->m_menus_by_filename.find(menuFilePath);
    if (alreadyLoadedMenuFile != conversionState->m_menus_by_filename.end())
    {
        std::cout << "Already loaded \"" << menuFilePath << "\", skipping\n";
        for (auto* menu : alreadyLoadedMenuFile->second)
        {
            menus.push_back(menu->Asset());
            menuListDependencies.push_back(menu);
        }
        return;
    }

    const auto menuFileResult = MenuLoader::ParseMenuFile(menuFilePath, searchPath, zoneState);
    if (menuFileResult)
    {
        MenuLoader::ProcessParsedResults(
            menuFilePath, searchPath, memory, manager, menuFileResult.get(), zoneState, conversionState, menus, menuListDependencies);
        if (!menuFileResult->m_menus_to_load.empty())
            std::cout << "WARNING: Menu file has menus to load even though it is not a menu list, ignoring: \"" << menuFilePath << "\"\n";
    }
    else
        std::cerr << "Could not read menu file \"" << menuFilePath << "\"\n";
}

bool AssetLoaderMenuList::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    std::vector<menuDef_t*> menus;
    std::vector<XAssetInfoGeneric*> menuListDependencies;

    auto* zoneState = manager->GetAssetLoadingContext()->GetZoneAssetLoaderState<menu::MenuAssetZoneState>();
    auto* conversionState = manager->GetAssetLoadingContext()->GetZoneAssetLoaderState<MenuConversionZoneState>();

    std::deque<std::string> menuLoadQueue;
    if (!BuildMenuFileQueue(menuLoadQueue, assetName, searchPath, memory, manager, zoneState, conversionState, menus, menuListDependencies))
        return false;

    while (!menuLoadQueue.empty())
    {
        const auto& menuFileToLoad = menuLoadQueue.front();

        LoadMenuFileFromQueue(menuFileToLoad, searchPath, memory, manager, zoneState, conversionState, menus, menuListDependencies);

        menuLoadQueue.pop_front();
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
