#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace string_table
{
    std::unique_ptr<AssetCreator<IW4::AssetStringTable>> CreateLoaderIW4(MemoryManager& memory, ISearchPath& searchPath);
} // namespace string_table
