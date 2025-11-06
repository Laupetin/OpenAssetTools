#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/BSP/BSP.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

namespace BSP
{
    class SkinnedVertsLinker
    {
    public:
        SkinnedVertsLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context);
        T6::SkinnedVertsDef* linkSkinnedVerts(BSPData* bsp);

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        AssetCreationContext& m_context;
    };
} // namespace BSP
