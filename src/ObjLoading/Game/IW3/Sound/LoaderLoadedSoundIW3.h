#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace loaded_sound
{
    std::unique_ptr<AssetCreator<IW3::AssetLoadedSound>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath);
} // namespace loaded_sound
