#include "LightDefDumperIW5.h"

#include "LightDef/LightDefCommon.h"

using namespace IW5;

namespace light_def
{
    void DumperIW5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetLightDef::Type>& asset)
    {
        const auto* lightDef = asset.Asset();
        const auto assetFile = context.OpenAssetFile(GetFileNameForAsset(asset.m_name));

        if (!assetFile || lightDef->attenuation.image == nullptr || lightDef->attenuation.image->name == nullptr || lightDef->cucoloris.image == nullptr
            || lightDef->cucoloris.image->name == nullptr)
            return;

        auto& stream = *assetFile;

        const auto* attenuationImageName = lightDef->attenuation.image->name;
        if (attenuationImageName[0] == ',')
            attenuationImageName = &attenuationImageName[1];

        const auto* cucolorisImageName = lightDef->cucoloris.image->name;
        if (cucolorisImageName[0] == ',')
            cucolorisImageName = &cucolorisImageName[1];

        stream << lightDef->attenuation.samplerState << attenuationImageName << '\0' << lightDef->cucoloris.samplerState << cucolorisImageName
               << static_cast<char>(lightDef->lmapLookupStart);
    }
} // namespace light_def
