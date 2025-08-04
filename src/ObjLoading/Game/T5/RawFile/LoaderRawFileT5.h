#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T5/T5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace T5::raw_file
{
    std::unique_ptr<AssetCreator<AssetRawFile>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace T5::raw_file
