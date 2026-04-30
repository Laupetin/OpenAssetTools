#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T5/T5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace light_def
{
    std::unique_ptr<AssetCreator<T5::AssetLightDef>> CreateLoaderT5(MemoryManager& memory, ISearchPath& searchPath);
} // namespace light_def
