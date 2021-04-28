#include "AssetDumperMapEnts.h"

using namespace IW3;

bool AssetDumperMapEnts::ShouldDump(XAssetInfo<MapEnts>* asset)
{
    return true;
}

bool AssetDumperMapEnts::CanDumpAsRaw()
{
    return true;
}

std::string AssetDumperMapEnts::GetFileNameForAsset(Zone* zone, XAssetInfo<MapEnts>* asset)
{
    return asset->m_name + ".ents";
}

void AssetDumperMapEnts::DumpRaw(AssetDumpingContext& context, XAssetInfo<MapEnts>* asset, std::ostream& stream)
{
    const auto* mapEnts = asset->Asset();
    stream.write(mapEnts->entityString, mapEnts->numEntityChars);
}
