#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace snd_alias_list
{
    std::unique_ptr<AssetCreator<IW4::AssetSound>> CreateLoaderIW4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace snd_alias_list
