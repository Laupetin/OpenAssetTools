#include "SkinnedVertsLinker.h"

namespace BSP
{
    SkinnedVertsLinker::SkinnedVertsLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
          m_search_path(searchPath),
          m_context(context)
    {
    }

    SkinnedVertsDef* SkinnedVertsLinker::linkSkinnedVerts(BSPData* bsp)
    {
        // maxSkinnedVerts defines how many model verts can be drawn at once
        // Low values cause models not to be drawn, so double origin's maxSkinnedVerts is used as a safe bet
        SkinnedVertsDef* skinnedVerts = m_memory.Alloc<SkinnedVertsDef>();
        skinnedVerts->name = m_memory.Dup("skinnedverts");
        skinnedVerts->maxSkinnedVerts = static_cast<unsigned int>(300000);

        return skinnedVerts;
    }
} // namespace BSP
