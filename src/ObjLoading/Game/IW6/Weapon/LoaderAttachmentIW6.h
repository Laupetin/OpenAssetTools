#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW6/IW6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace attachment
{
    std::unique_ptr<AssetCreator<IW6::AssetAttachment>> CreateLoaderIW6(MemoryManager& memory, ISearchPath& searchPath);
} // namespace attachment
