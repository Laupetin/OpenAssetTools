#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Zone/Zone.h"

#include <memory>

namespace localize
{
    std::unique_ptr<AssetCreator<IW4::AssetLocalize>> CreateLoaderIW4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace localize
