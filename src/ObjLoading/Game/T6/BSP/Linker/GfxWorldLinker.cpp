#include "GfxWorldLinker.h"

namespace BSP
{
    GfxWorldLinker::GfxWorldLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
        m_search_path(searchPath),
        m_context(context)
    {
    }

    AssetCreationResult GfxWorldLinker::linkGfxWorld(BSPData* bsp)
    {
        return AssetCreationResult::Failure();
    }
}

