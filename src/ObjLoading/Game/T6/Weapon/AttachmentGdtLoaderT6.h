#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace T6::attachment
{
    std::unique_ptr<AssetCreator<AssetAttachment>> CreateGdtLoader(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone);
} // namespace T6::attachment
