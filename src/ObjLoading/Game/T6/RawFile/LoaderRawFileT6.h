#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace T6::raw_file
{
    std::unique_ptr<AssetCreator<AssetRawFile>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace T6::raw_file
