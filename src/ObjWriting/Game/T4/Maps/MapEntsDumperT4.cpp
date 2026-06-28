#include "MapEntsDumperT4.h"

using namespace T4;

namespace map_ents
{
    void DumperT4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<MapEnts>& asset)
    {
        const auto* mapEnts = asset.Asset();
        const auto assetFile = context.OpenAssetFile(asset.m_name + ".ents");

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(mapEnts->entityString, mapEnts->numEntityChars);
    }
} // namespace map_ents
