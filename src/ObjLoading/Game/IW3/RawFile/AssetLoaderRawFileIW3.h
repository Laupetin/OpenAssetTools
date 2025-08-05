#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace raw_file
{
    std::unique_ptr<AssetCreator<IW3::AssetRawFile>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath);
} // namespace raw_file
