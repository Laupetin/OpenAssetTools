#define NOMINMAX
#include "AssetDumperAddonMapEnts.h"

#include <algorithm>

using namespace IW4;

bool AssetDumperAddonMapEnts::ShouldDump(XAssetInfo<AddonMapEnts>* asset)
{
    return true;
}

std::string AssetDumperAddonMapEnts::GetFileNameForAsset(Zone* zone, XAssetInfo<AddonMapEnts>* asset)
{
    return asset->m_name;
}

void AssetDumperAddonMapEnts::DumpAsset(Zone* zone, XAssetInfo<AddonMapEnts>* asset, std::ostream& stream)
{
    const auto* addonMapEnts = asset->Asset();
    stream.write(addonMapEnts->entityString, std::max(addonMapEnts->numEntityChars - 1, 0));
}
