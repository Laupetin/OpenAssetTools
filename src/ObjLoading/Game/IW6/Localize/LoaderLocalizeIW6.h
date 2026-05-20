#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW6/IW6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Zone/Zone.h"

#include <memory>

namespace localize
{
    std::unique_ptr<AssetCreator<IW6::AssetLocalize>> CreateLoaderIW6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace localize
