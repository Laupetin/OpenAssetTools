#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace IW4::material
{
    std::unique_ptr<AssetCreator<AssetMaterial>> CreateCompiler(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt);
} // namespace IW4::material
