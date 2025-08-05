#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace light_def
{
    std::unique_ptr<AssetCreator<IW4::AssetLightDef>> CreateLoaderIW4(MemoryManager& memory, ISearchPath& searchPath);
} // namespace light_def
