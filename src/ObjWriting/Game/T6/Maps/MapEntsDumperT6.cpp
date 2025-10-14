#include "MapEntsDumperT6.h"

#include <format>

using namespace T6;

namespace map_ents
{
    void DumperT6::DumpAsset(AssetDumpingContext& context, const XAssetInfo<MapEnts>& asset)
    {
        const auto* mapEnts = asset.Asset();

        const auto mapEntsFile = context.OpenAssetFile(std::format("{}.ents", mapEnts->name));

        if (!mapEntsFile)
            return;

        auto& stream = *mapEntsFile;
        stream.write(mapEnts->entityString, mapEnts->numEntityChars - 1);
    }
} // namespace map_ents
