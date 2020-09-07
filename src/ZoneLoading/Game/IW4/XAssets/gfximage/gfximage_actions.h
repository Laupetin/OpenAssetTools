#pragma once

#include "Loading/AssetLoadingActions.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class Actions_GfxImage final : public AssetLoadingActions
    {
    public:
        explicit Actions_GfxImage(Zone* zone);

        void OnImageLoaded(GfxImage* image) const;
        void LoadImageData(GfxImageLoadDef* loadDef, GfxImage* image) const;
    };
}
