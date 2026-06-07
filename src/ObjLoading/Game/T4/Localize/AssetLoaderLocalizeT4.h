#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T4/T4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Zone/Zone.h"

#include <memory>

namespace localize
{
    std::unique_ptr<AssetCreator<T4::AssetLocalize>> CreateLoaderT4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace localize
