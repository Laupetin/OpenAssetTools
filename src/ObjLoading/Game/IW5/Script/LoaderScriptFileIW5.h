#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW5/IW5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace IW5::script
{
    std::unique_ptr<AssetCreator<AssetScript>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace IW5::script
