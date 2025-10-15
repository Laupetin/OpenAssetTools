#define NOMINMAX
#include "AddonMapEntsDumperIW4.h"

#include <algorithm>

using namespace IW4;

namespace addon_map_ents
{
    DumperIW4::DumperIW4(const AssetPool<AssetAddonMapEnts::Type>& pool)
        : AbstractAssetDumper(pool)
    {
    }

    void DumperIW4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetAddonMapEnts::Type>& asset)
    {
        const auto* addonMapEnts = asset.Asset();
        const auto assetFile = context.OpenAssetFile(asset.m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;

        stream.write(addonMapEnts->entityString, std::max(addonMapEnts->numEntityChars - 1, 0));
    }
} // namespace addon_map_ents
