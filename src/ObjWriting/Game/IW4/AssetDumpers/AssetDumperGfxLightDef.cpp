#include "AssetDumperGfxLightDef.h"

#include <sstream>

using namespace IW4;

std::string AssetDumperGfxLightDef::GetAssetFilename(const std::string& assetName)
{
    std::ostringstream ss;

    ss << "lights/" << assetName;

    return ss.str();
}

bool AssetDumperGfxLightDef::ShouldDump(XAssetInfo<GfxLightDef>* asset)
{
    return true;
}

void AssetDumperGfxLightDef::DumpAsset(AssetDumpingContext& context, XAssetInfo<GfxLightDef>* asset)
{
    const auto* lightDef = asset->Asset();
    const auto assetFile = context.OpenAssetFile(GetAssetFilename(asset->m_name));

    if (!assetFile || lightDef->attenuation.image == nullptr || lightDef->attenuation.image->name == nullptr)
        return;

    auto& stream = *assetFile;

    const auto* imageName = lightDef->attenuation.image->name;
    if (imageName[0] == ',')
        imageName = &imageName[1];

    stream << lightDef->attenuation.samplerState << imageName << static_cast<char>(lightDef->lmapLookupStart);
}
