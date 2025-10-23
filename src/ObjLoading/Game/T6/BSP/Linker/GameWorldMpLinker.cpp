#include "GameWorldMpLinker.h"

namespace BSP
{
    GameWorldMpLinker::GameWorldMpLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
        m_search_path(searchPath),
        m_context(context)
    {
    }

    AssetCreationResult GameWorldMpLinker::linkGameWorldMp(BSPData* bsp)
    {
        GameWorldMp* gameWorldMp = m_memory.Alloc<GameWorldMp>();

        gameWorldMp->name = m_memory.Dup(bsp->bspName.c_str());

        gameWorldMp->path.nodeCount = 0;
        gameWorldMp->path.originalNodeCount = 0;
        gameWorldMp->path.visBytes = 0;
        gameWorldMp->path.smoothBytes = 0;
        gameWorldMp->path.nodeTreeCount = 0;

        int nodeCount = gameWorldMp->path.nodeCount + 128;
        gameWorldMp->path.nodes = m_memory.Alloc<pathnode_t>(nodeCount);
        gameWorldMp->path.basenodes = m_memory.Alloc<pathbasenode_t>(nodeCount);
        memset(gameWorldMp->path.nodes, 0, nodeCount * sizeof(pathnode_t));
        memset(gameWorldMp->path.basenodes, 0, nodeCount * sizeof(pathbasenode_t));

        gameWorldMp->path.pathVis = nullptr;
        gameWorldMp->path.smoothCache = nullptr;
        gameWorldMp->path.nodeTree = nullptr;
        
        auto gameWorldMpAsset = m_context.AddAsset<AssetGameWorldMp>(gameWorldMp->name, gameWorldMp);
        return AssetCreationResult::Success(gameWorldMpAsset);
    }
}

