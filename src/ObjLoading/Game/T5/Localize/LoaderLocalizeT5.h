#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T5/T5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Zone/Zone.h"

#include <memory>

namespace T5
{
    std::unique_ptr<AssetCreator<AssetLocalize>> CreateLocalizeLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace T5
