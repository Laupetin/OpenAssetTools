#include "LightDefDumperIW5.h"

#include "LightDef/LightDefCommon.h"

#include <assert.h>

using namespace IW5;

namespace light_def
{
    void DumperIW5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetLightDef::Type>& asset)
    {
        const auto* lightDef = asset.Asset();
        const auto assetFile = context.OpenAssetFile(GetFileNameForAsset(asset.m_name));
        auto& stream = *assetFile;

        assert(lightDef->attenuation.image != nullptr);
        const auto* attenuationImageName = lightDef->attenuation.image->name;
        if (attenuationImageName && attenuationImageName[0] == ',')
            attenuationImageName = &attenuationImageName[1];

        const auto* cucolorisImageName = "";
        if (lightDef->cucoloris.image)
        {
            cucolorisImageName = lightDef->cucoloris.image->name;
            if (cucolorisImageName && cucolorisImageName[0] == ',')
                cucolorisImageName = &cucolorisImageName[1];
        }

        stream << lightDef->attenuation.samplerState << attenuationImageName << '\0' << lightDef->cucoloris.samplerState << cucolorisImageName;
    }
} // namespace light_def
