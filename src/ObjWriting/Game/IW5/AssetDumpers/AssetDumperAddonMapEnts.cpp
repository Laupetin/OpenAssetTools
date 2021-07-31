#define NOMINMAX
#include "AssetDumperAddonMapEnts.h"

#include <algorithm>

using namespace IW5;

bool AssetDumperAddonMapEnts::ShouldDump(XAssetInfo<AddonMapEnts>* asset)
{
    return true;
}

void AssetDumperAddonMapEnts::DumpAsset(AssetDumpingContext& context, XAssetInfo<AddonMapEnts>* asset)
{
    const auto* addonMapEnts = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile)
        return;

    auto& stream = *assetFile;
    stream.write(addonMapEnts->entityString, std::max(addonMapEnts->numEntityChars - 1, 0));
}
