#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T5/T5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace font
{
    std::unique_ptr<AssetCreator<T5::AssetFont>> CreateLoaderT5(MemoryManager& memory, ISearchPath& searchPath);
} // namespace font
