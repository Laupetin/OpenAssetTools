#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace fx_effect
{
    std::unique_ptr<AssetCreator<T6::AssetFx>> CreateLoaderT6(MemoryManager& memory, ISearchPath& searchPath);
} // namespace fx_effect