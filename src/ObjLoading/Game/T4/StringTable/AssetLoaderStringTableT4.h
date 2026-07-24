#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T4/T4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace string_table
{
    std::unique_ptr<AssetCreator<T4::AssetStringTable>> CreateLoaderT4(MemoryManager& memory, ISearchPath& searchPath);
} // namespace string_table
