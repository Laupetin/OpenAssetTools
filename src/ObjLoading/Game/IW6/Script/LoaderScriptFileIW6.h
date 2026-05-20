#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW6/IW6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace script
{
    std::unique_ptr<AssetCreator<IW6::AssetScript>> CreateLoaderIW6(MemoryManager& memory, ISearchPath& searchPath);
} // namespace script
