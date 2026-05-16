#include "LightDefAssetCreationStateIW3.h"

#include "Utils/Logging/Log.h"

using namespace IW3;

namespace
{
    constexpr auto MAX_LMAP_USAGE = 512;
};

LightDefAssetCreationState::LightDefAssetCreationState()
    : m_lmap_pixels_used_for_falloff(0)
{
}

void LightDefAssetCreationState::SetLightDefLookupStart(GfxLightDef* lightDef, AssetCreationContext& context)
{
    const auto* image = lightDef->attenuation.image;
    if (!image)
        return;

    // We need the actual image to determine the width, so force load it, if it is a reference
    if (image->name && image->name[0] == ',')
    {
        const auto forceLoadedImage = context.ForceLoadDependency<AssetImage>(&image->name[1]);
        if (!forceLoadedImage)
            return;

        image = forceLoadedImage->Asset();
    }

    const int newLmapUsage = image->width + 2;
    if (m_lmap_pixels_used_for_falloff + newLmapUsage > MAX_LMAP_USAGE)
    {
        con::error(
            "Total pixel width of all attenuation textures plus 2 border pixels is {} > {}", m_lmap_pixels_used_for_falloff + newLmapUsage, MAX_LMAP_USAGE);
    }

    lightDef->lmapLookupStart = m_lmap_pixels_used_for_falloff + 1;
    m_lmap_pixels_used_for_falloff += newLmapUsage;
}
