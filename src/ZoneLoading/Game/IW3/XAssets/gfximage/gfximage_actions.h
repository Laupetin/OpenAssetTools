#pragma once

#include "Loading/AssetLoadingActions.h"
#include "Game/IW3/IW3.h"

namespace IW3
{
    class Actions_GfxImage final : public AssetLoadingActions
    {
    public:
        explicit Actions_GfxImage(Zone* zone);

        void OnImageLoaded(GfxImage* image) const;
        void LoadImageData(GfxImageLoadDef* loadDef, GfxImage* image) const;
    };
}
