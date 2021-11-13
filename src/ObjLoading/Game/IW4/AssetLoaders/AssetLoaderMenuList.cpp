#include "AssetLoaderMenuList.h"

#include <cstring>
#include <iostream>

#include "Game/IW4/IW4.h"
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
        zoneState->m_loaded_files.emplace(menuFilePath);
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

void AssetLoaderMenuList::ProcessParsedResults(const std::string& assetName, MemoryManager* memory, IAssetLoadingManager* manager, menu::ParsingResult* parsingResult,
                                               menu::MenuAssetZoneState* zoneState)
{
    std::cout << "Successfully read menu list \"" << assetName << "\":\n";

    std::cout << "  " << parsingResult->m_menus_to_load.size() << " menus to load:\n";
    for (const auto& menuToLoad : parsingResult->m_menus_to_load)
        std::cout << "    " << menuToLoad << "\n";

    std::cout << "  " << parsingResult->m_menus.size() << " menus:\n";
    for (const auto& menu : parsingResult->m_menus)
        std::cout << "    " << menu->m_name << " (" << menu->m_items.size() << " items)\n";

    std::cout << "  " << parsingResult->m_functions.size() << " functions:\n";
    for (const auto& function : parsingResult->m_functions)
        std::cout << "    " << function->m_name << "\n";
}

bool AssetLoaderMenuList::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    std::deque<std::string> menuFileQueue;

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

        const auto menuFileResult = reader.ReadMenuFile();
        if (menuFileResult)
        {
            ProcessParsedResults(nextMenuFile, memory, manager, menuFileResult.get(), zoneState);
            AddMenuFilesToLoadToQueue(menuFileQueue, menuFileResult.get(), zoneState);
        }
        else
            std::cout << "Could not read menu list \"" << nextMenuFile << "\"\n";

        menuFileQueue.pop_front();
    }

    return true;
}
