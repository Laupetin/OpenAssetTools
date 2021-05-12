#pragma once

#include "Loading/AssetLoadingActions.h"
#include "Game/T5/T5.h"

namespace T5
{
    class Actions_GfxImage final : public AssetLoadingActions
    {
    public:
        explicit Actions_GfxImage(Zone* zone);

        void OnImageLoaded(GfxImage* image) const;
        void LoadImageData(GfxImageLoadDef* loadDef, GfxImage* image) const;
    };
}
