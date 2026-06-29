#include "MapEntsDumperT6.h"

#include "Maps/MapEntsCommon.h"

using namespace T6;

namespace map_ents
{
    void DumperT6::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetMapEnts::Type>& asset)
    {
        const auto* mapEnts = asset.Asset();
        const auto assetFile = context.OpenAssetFile(GetEntsFileNameForAssetName(asset.m_name));

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(mapEnts->entityString, mapEnts->numEntityChars - 1);
    }
} // namespace map_ents
