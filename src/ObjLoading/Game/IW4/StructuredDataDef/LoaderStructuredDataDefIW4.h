#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace structured_data_def
{
    std::unique_ptr<AssetCreator<IW4::AssetStructuredDataDef>> CreateLoaderIW4(MemoryManager& memory, ISearchPath& searchPath);
} // namespace structured_data_def
