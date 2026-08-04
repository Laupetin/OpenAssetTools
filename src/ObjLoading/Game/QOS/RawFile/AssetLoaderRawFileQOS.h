#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/QOS/QOS.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace raw_file
{
    std::unique_ptr<AssetCreator<QOS::AssetRawFile>> CreateLoaderQOS(MemoryManager& memory, ISearchPath& searchPath);
} // namespace raw_file
