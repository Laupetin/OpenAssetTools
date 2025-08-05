#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace sound
{
    std::unique_ptr<AssetCreator<T6::AssetSoundBank>> CreateSoundBankLoaderT6(MemoryManager& memory, ISearchPath& searchPath);
} // namespace sound
