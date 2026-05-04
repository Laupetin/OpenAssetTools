#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/IZoneAssetCreationState.h"
#include "Game/IW5/IW5.h"

namespace IW5
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
} // namespace IW5
