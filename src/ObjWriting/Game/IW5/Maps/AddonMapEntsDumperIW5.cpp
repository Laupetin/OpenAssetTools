#define NOMINMAX
#include "AddonMapEntsDumperIW5.h"

#include <algorithm>

using namespace IW5;

namespace IW5::addon_map_ents
{
    bool Dumper::ShouldDump(XAssetInfo<AddonMapEnts>* asset)
    {
        return true;
    }

    void Dumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<AddonMapEnts>* asset)
    {
        const auto* addonMapEnts = asset->Asset();
        const auto assetFile = context.OpenAssetFile(asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(addonMapEnts->entityString, std::max(addonMapEnts->numEntityChars - 1, 0));
    }
} // namespace IW5::addon_map_ents
