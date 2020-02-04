#pragma once

#include "Loading/AssetLoadingActions.h"
#include "Game/T6/T6.h"

class Actions_GfxImage final : public AssetLoadingActions
{
public:
    explicit Actions_GfxImage(Zone* zone);

    void LoadImageData(T6::GfxImageLoadDef* loadDef, T6::GfxImage* image) const;
};