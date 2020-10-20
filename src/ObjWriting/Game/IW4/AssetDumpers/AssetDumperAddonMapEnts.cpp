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

void AssetDumperAddonMapEnts::DumpAsset(Zone* zone, XAssetInfo<AddonMapEnts>* asset, FileAPI::File* out)
{
    const auto* addonMapEnts = asset->Asset();
    out->Write(addonMapEnts->entityString, 1, std::max(addonMapEnts->numEntityChars - 1, 0));
}
