#include "MapEntsDumperT6.h"

#include <format>

using namespace T6;

namespace T6::map_ents
{
    bool Dumper::ShouldDump(XAssetInfo<MapEnts>* asset)
    {
        return true;
    }

    void Dumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<MapEnts>* asset)
    {
        const auto* mapEnts = asset->Asset();

        const auto mapEntsFile = context.OpenAssetFile(std::format("{}.ents", mapEnts->name));

        if (!mapEntsFile)
            return;

        auto& stream = *mapEntsFile;
        stream.write(mapEnts->entityString, mapEnts->numEntityChars - 1);
    }
} // namespace T6::map_ents
