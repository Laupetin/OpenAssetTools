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

    MenuFileReader reader(*file.m_stream, assetName, [searchPath](const std::string& filename) -> std::unique_ptr<std::istream>
    {
        auto foundFileToInclude = searchPath->Open(filename);
        if (!foundFileToInclude.IsOpen() || !foundFileToInclude.m_stream)
            return nullptr;

        return std::move(foundFileToInclude.m_stream);
    });

    if(!reader.ReadMenuFile(MenuFeatureLevel::IW4))
    {
        std::cout << "Could not read menu list \"" << assetName << "\"\n";
    }

    return true;
}
