#include "SkinnedVertsLinker.h"

namespace BSP
{
    SkinnedVertsLinker::SkinnedVertsLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
          m_search_path(searchPath),
          m_context(context)
    {
    }

    AssetCreationResult SkinnedVertsLinker::linkSkinnedVerts(BSPData* bsp)
    {
        std::string assetName = "skinnedverts";

        // I'm pretty sure maxSkinnedVerts relates to the max amount of xmodel skinned verts a map will have
        // But setting it to the world vertex count seems to work
        SkinnedVertsDef* skinnedVerts = m_memory.Alloc<SkinnedVertsDef>();
        skinnedVerts->name = m_memory.Dup(assetName.c_str());
        skinnedVerts->maxSkinnedVerts = static_cast<unsigned int>(bsp->gfxWorld.vertices.size());

        auto skinnedVertsAsset = m_context.AddAsset<AssetSkinnedVerts>(assetName, skinnedVerts);
        return AssetCreationResult::Success(skinnedVertsAsset);
    }
} // namespace BSP
