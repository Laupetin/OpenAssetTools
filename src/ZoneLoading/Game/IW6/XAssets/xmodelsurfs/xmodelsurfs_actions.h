#pragma once

#include "Game/IW6/IW6.h"
#include "Loading/AssetLoadingActions.h"

namespace IW6
{
    class Actions_XModelSurfs final : public AssetLoadingActions
    {
    public:
        explicit Actions_XModelSurfs(Zone& zone);

        void ReallocGfxVertexUnion0(const GfxVertexUnion0* vertexUnion, XSurface* surface) const;
        void ReallocXBlendInfo(const XBlendInfo* blendInfo, XSurface* surface) const;
    };
} // namespace IW6
