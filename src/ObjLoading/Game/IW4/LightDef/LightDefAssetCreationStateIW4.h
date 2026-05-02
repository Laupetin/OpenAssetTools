#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "Asset/AssetCreationContext.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    constexpr auto MAX_LMAP_USAGE = 512;

    class LightDefAssetCreationState : public IZoneAssetCreationState
    {
    public:
        LightDefAssetCreationState();

        void SetLightDefLookupStart(GfxLightDef* lightDef, AssetCreationContext& context);

    private:
        int m_lmap_pixels_used_for_falloff;
    };
} // namespace
