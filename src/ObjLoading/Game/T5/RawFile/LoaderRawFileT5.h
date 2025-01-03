#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T5/T5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace T5
{
    std::unique_ptr<AssetCreator<AssetRawFile>> CreateRawFileLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace T5
