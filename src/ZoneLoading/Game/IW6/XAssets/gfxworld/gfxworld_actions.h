#pragma once

#include "Game/IW6/IW6.h"
#include "Loading/AssetLoadingActions.h"

namespace IW6
{
    class Actions_GfxWorld final : public AssetLoadingActions
    {
    public:
        explicit Actions_GfxWorld(Zone& zone);

        void ReallocGfxAabbTree(const GfxAabbTree* aabbTree, GfxWorld* gfxWorld) const;
    };
} // namespace IW6
