#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW5/IW5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace loaded_sound
{
    std::unique_ptr<AssetCreator<IW5::AssetLoadedSound>> CreateLoaderIW5(MemoryManager& memory, ISearchPath& searchPath);
} // namespace loaded_sound
