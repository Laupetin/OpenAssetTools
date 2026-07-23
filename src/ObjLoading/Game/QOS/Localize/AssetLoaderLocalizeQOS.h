#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/QOS/QOS.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Zone/Zone.h"

#include <memory>

namespace localize
{
    std::unique_ptr<AssetCreator<QOS::AssetLocalize>> CreateLoaderQOS(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace localize
