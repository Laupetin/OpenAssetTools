#define NOMINMAX
#include "AssetDumperAddonMapEnts.h"

#include <algorithm>

using namespace IW4;

bool AssetDumperAddonMapEnts::ShouldDump(AddonMapEnts* asset)
{
    return true;
}

std::string AssetDumperAddonMapEnts::GetFileNameForAsset(Zone* zone, AddonMapEnts* asset)
{
    return std::string(asset->name);
}

void AssetDumperAddonMapEnts::DumpAsset(Zone* zone, AddonMapEnts* asset, FileAPI::File* out)
{
    out->Write(asset->entityString, 1, std::max(asset->numEntityChars - 1, 0));
}
