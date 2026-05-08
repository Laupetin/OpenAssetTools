#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace loaded_sound
{
    std::unique_ptr<AssetCreator<IW3::AssetLoadedSound>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace loaded_sound
