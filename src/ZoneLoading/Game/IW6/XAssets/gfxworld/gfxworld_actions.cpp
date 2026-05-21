#include "gfxworld_actions.h"

#include <cassert>
#include <cstring>

using namespace IW6;

Actions_GfxWorld::Actions_GfxWorld(Zone& zone)
    : AssetLoadingActions(zone)
{
}

void Actions_GfxWorld::ReallocGfxAabbTree(const GfxAabbTree* aabbTree, GfxWorld* gfxWorld) const
{
    assert(aabbTree);
    assert(gfxWorld);
    assert(gfxWorld->aabbTreeCounts);

    gfxWorld->aabbTrees = m_zone.Memory().Alloc<GfxCellTree>(gfxWorld->aabbTreeCounts->aabbTreeCount);
    memcpy(gfxWorld->aabbTrees, aabbTree, sizeof(GfxCellTree) * gfxWorld->aabbTreeCounts->aabbTreeCount);
}
