#include "AssetDumperMapEnts.h"

#include <filesystem>

using namespace T6;

bool AssetDumperMapEnts::ShouldDump(XAssetInfo<MapEnts>* asset)
{
    return true;
}

void AssetDumperMapEnts::DumpAsset(AssetDumpingContext& context, XAssetInfo<MapEnts>* asset)
{
    const auto* mapEnts = asset->Asset();

    std::filesystem::path mapEntsPath(mapEnts->name);
    mapEntsPath.replace_extension("ents");

    const auto mapEntsFile = context.OpenAssetFile(mapEntsPath.string());

    if (!mapEntsFile)
        return;

    auto& stream = *mapEntsFile;
    stream.write(mapEnts->entityString, mapEnts->numEntityChars - 1);
}
