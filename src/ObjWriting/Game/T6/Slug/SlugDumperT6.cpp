#include "SlugDumperT6.h"

using namespace T6;

namespace slug
{
    DumperT6::DumperT6(const AssetPool<AssetSlug::Type>& pool)
        : AbstractAssetDumper(pool)
    {
    }

    void DumperT6::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetSlug::Type>& asset)
    {
        const auto* slug = asset.Asset();
        const auto assetFile = context.OpenAssetFile(asset.m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(slug->buffer, slug->len);
    }
} // namespace slug
