#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace z_barrier
{
    std::unique_ptr<AssetCreator<T6::AssetZBarrier>> CreateGdtLoaderT6(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone);
} // namespace z_barrier
