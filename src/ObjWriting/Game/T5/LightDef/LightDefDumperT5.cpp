#include "LightDefDumperT5.h"

#include "LightDef/LightDefCommon.h"

using namespace T5;

namespace light_def
{
    void DumperT5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetLightDef::Type>& asset)
    {
        const auto* lightDef = asset.Asset();
        const auto assetFile = context.OpenAssetFile(GetFileNameForAsset(asset.m_name));
        if (!assetFile)
        {
            con::error("Could not open GfxLightDef file for dumping!");
            return;
        }

        if (lightDef->attenuation.image == nullptr || lightDef->attenuation.image->name == nullptr)
        {
            con::error("GfxLightDef attenuation data was invalid!");
            return;
        }

        auto& stream = *assetFile;

        const auto* imageName = lightDef->attenuation.image->name;
        if (imageName[0] == ',')
            imageName = &imageName[1];

        stream << lightDef->attenuation.samplerState << imageName << '\0';
    }
} // namespace light_def
