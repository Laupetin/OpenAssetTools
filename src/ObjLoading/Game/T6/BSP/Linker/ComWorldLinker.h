#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/BSP/BSP.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace BSP
{
    class ComWorldLinker
    {
    public:
        ComWorldLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context);
        [[nodiscard]] T6::ComWorld* linkComWorld(const BSPData& bsp) const;

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;
    };
} // namespace BSP
