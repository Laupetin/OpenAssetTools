#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW5/IW5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace snd_alias_list
{
    std::unique_ptr<AssetCreator<IW5::AssetSound>> CreateLoaderIW5(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace snd_alias_list
