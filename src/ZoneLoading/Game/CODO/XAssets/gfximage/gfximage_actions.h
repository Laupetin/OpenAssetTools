#pragma once

#include "Game/CODO/CODO.h"
#include "Loading/AssetLoadingActions.h"

namespace CODO
{
    class Actions_GfxImage final : public AssetLoadingActions
    {
    public:
        explicit Actions_GfxImage(Zone& zone);

        void OnImageLoaded(GfxImage* image) const;
        void LoadImageData(GfxImageLoadDef* loadDef, GfxImage* image) const;
    };
} // namespace CODO
