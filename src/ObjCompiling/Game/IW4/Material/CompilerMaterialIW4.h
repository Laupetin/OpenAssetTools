#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace material
{
    std::unique_ptr<AssetCreator<IW4::AssetMaterial>> CreateCompilerIW4(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt);
} // namespace material
