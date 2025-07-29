#include "MapEntsDumperIW3.h"

using namespace IW3;

namespace IW3::map_ents
{
    bool Dumper::ShouldDump(XAssetInfo<MapEnts>* asset)
    {
        return true;
    }

    void Dumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<MapEnts>* asset)
    {
        const auto* mapEnts = asset->Asset();
        const auto assetFile = context.OpenAssetFile(asset->m_name + ".ents");

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(mapEnts->entityString, mapEnts->numEntityChars);
    }
} // namespace IW3::map_ents
