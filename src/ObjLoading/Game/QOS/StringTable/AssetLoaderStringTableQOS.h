#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/QOS/QOS.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace string_table
{
    std::unique_ptr<AssetCreator<QOS::AssetStringTable>> CreateLoaderQOS(MemoryManager& memory, ISearchPath& searchPath);
} // namespace string_table
