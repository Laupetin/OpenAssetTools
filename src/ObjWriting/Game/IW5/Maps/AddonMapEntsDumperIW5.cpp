#define NOMINMAX
#include "AddonMapEntsDumperIW5.h"

#include <algorithm>

using namespace IW5;

namespace addon_map_ents
{
    DumperIW5::DumperIW5(const AssetPool<AssetAddonMapEnts::Type>& pool)
        : AbstractAssetDumper(pool)
    {
    }

    void DumperIW5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetAddonMapEnts::Type>& asset)
    {
        const auto* addonMapEnts = asset.Asset();
        const auto assetFile = context.OpenAssetFile(asset.m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(addonMapEnts->entityString, std::max(addonMapEnts->numEntityChars - 1, 0));
    }
} // namespace addon_map_ents
