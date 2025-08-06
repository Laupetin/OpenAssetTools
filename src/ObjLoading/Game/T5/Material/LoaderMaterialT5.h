#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T5/T5.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace material
{
    std::unique_ptr<AssetCreator<T5::AssetMaterial>> CreateLoaderT5(MemoryManager& memory, ISearchPath& searchPath);
} // namespace material
