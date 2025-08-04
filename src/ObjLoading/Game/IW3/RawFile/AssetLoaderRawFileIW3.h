#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace IW3::raw_file
{
    std::unique_ptr<AssetCreator<AssetRawFile>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace IW3::raw_file
