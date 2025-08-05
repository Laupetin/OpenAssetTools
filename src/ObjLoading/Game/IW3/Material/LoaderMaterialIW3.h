#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace material
{
    std::unique_ptr<AssetCreator<IW3::AssetMaterial>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath);
} // namespace material
