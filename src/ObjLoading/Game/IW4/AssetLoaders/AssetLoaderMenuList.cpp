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

bool AssetLoaderMenuList::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(assetName);
    if (!file.IsOpen())
        return false;

    MenuFileReader reader(*file.m_stream, assetName, MenuFeatureLevel::IW4, [searchPath](const std::string& filename, const std::string& sourceFile) -> std::unique_ptr<std::istream>
    {
        auto foundFileToInclude = searchPath->Open(filename);
        if (!foundFileToInclude.IsOpen() || !foundFileToInclude.m_stream)
            return nullptr;

        return std::move(foundFileToInclude.m_stream);
    });

    const auto menuFileResult = reader.ReadMenuFile();
    if(menuFileResult)
    {
        std::cout << "Successfully read menu list \"" << assetName << "\":\n";

        std::cout << "  " << menuFileResult->m_menus_to_load.size() << " menus to load:\n";
        for (const auto& menuToLoad : menuFileResult->m_menus_to_load)
            std::cout << "    " << menuToLoad << "\n";

        std::cout << "  " << menuFileResult->m_menus.size() << " menus:\n";
        for (const auto& menu : menuFileResult->m_menus)
            std::cout << "    " << menu->m_name << " (" << menu->m_items.size() << " items)\n";

        std::cout << "  " << menuFileResult->m_functions.size() << " functions:\n";
        for (const auto& function : menuFileResult->m_functions)
            std::cout << "    " << function->m_name << "\n";
    }
    else
        std::cout << "Could not read menu list \"" << assetName << "\"\n";

    return true;
}
