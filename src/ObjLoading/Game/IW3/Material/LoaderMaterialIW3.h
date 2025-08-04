#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace IW3::material
{
    std::unique_ptr<AssetCreator<AssetMaterial>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace IW3::material
