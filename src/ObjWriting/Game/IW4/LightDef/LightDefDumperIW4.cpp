#include "LightDefDumperIW4.h"

#include "LightDef/LightDefCommon.h"

using namespace IW4;

namespace light_def
{
    DumperIW4::DumperIW4(const AssetPool<AssetLightDef::Type>& pool)
        : AbstractAssetDumper(pool)
    {
    }

    void DumperIW4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetLightDef::Type>& asset)
    {
        const auto* lightDef = asset.Asset();
        const auto assetFile = context.OpenAssetFile(GetFileNameForAsset(asset.m_name));

        if (!assetFile || lightDef->attenuation.image == nullptr || lightDef->attenuation.image->name == nullptr)
            return;

        auto& stream = *assetFile;

        const auto* imageName = lightDef->attenuation.image->name;
        if (imageName[0] == ',')
            imageName = &imageName[1];

        stream << lightDef->attenuation.samplerState << imageName << static_cast<char>(lightDef->lmapLookupStart);
    }
} // namespace light_def
