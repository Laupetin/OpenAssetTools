#include "AssetDumperMapEnts.h"

#include <format>

using namespace T6;

bool AssetDumperMapEnts::ShouldDump(XAssetInfo<MapEnts>* asset)
{
    return true;
}

void AssetDumperMapEnts::DumpAsset(AssetDumpingContext& context, XAssetInfo<MapEnts>* asset)
{
    const auto* mapEnts = asset->Asset();

    const auto mapEntsFile = context.OpenAssetFile(std::format("{}.ents", mapEnts->name));

    if (!mapEntsFile)
        return;

    auto& stream = *mapEntsFile;
    stream.write(mapEnts->entityString, mapEnts->numEntityChars - 1);
}
