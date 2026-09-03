#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T4/T4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace menu
{
    std::unique_ptr<AssetCreator<T4::AssetMenuList>> CreateMenuListLoaderT4(MemoryManager& memory, ISearchPath& searchPath);
}
