#pragma once

#include "Game/IW5/IW5.h"
#include "Loading/AssetLoadingActions.h"

namespace IW5
{
    class Actions_GfxImage final : public AssetLoadingActions
    {
    public:
        explicit Actions_GfxImage(Zone* zone);

        void OnImageLoaded(GfxImage* image) const;
        void LoadImageData(GfxImageLoadDef* loadDef, GfxImage* image) const;
    };
} // namespace IW5
