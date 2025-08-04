#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T5/T5.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace T5::material
{
    std::unique_ptr<AssetCreator<AssetMaterial>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath);
} // namespace T5::material
