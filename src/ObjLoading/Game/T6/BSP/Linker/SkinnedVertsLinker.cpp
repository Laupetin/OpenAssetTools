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
        // But setting it to the world vertex count seems to work
        SkinnedVertsDef* skinnedVerts = m_memory.Alloc<SkinnedVertsDef>();
        skinnedVerts->name = m_memory.Dup("skinnedverts");
        skinnedVerts->maxSkinnedVerts = static_cast<unsigned int>(bsp->gfxWorld.vertices.size());

        return skinnedVerts;
    }
} // namespace BSP
