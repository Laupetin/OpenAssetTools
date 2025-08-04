#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace IW4::raw_file
{
    std::unique_ptr<AssetCreator<AssetRawFile>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace IW4::raw_file
