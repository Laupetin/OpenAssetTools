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
        T6::ComWorld* linkComWorld(BSPData* bsp);

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;
    };
} // namespace BSP
