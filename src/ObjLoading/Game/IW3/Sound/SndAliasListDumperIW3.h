#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace snd_alias
{
    std::unique_ptr<AssetCreator<IW3::AssetSound>> CreateRawLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace snd_alias
