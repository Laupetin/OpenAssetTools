#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace IW4
{
    std::unique_ptr<AssetCreator<AssetStructuredDataDef>> CreateStructuredDataDefLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace IW4
