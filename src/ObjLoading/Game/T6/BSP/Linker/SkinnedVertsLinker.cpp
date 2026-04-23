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
        // Pretty sure maxSkinnedVerts relates to the max amount of xmodel skinned verts a map will have
        SkinnedVertsDef* skinnedVerts = m_memory.Alloc<SkinnedVertsDef>();
        skinnedVerts->name = m_memory.Dup("skinnedverts");
        skinnedVerts->maxSkinnedVerts = static_cast<unsigned int>(bsp->gfxWorld.xmodels.size() * 1000);

        return skinnedVerts;
    }
} // namespace BSP
