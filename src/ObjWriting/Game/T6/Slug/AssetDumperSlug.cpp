#include "AssetDumperSlug.h"

using namespace T6;

bool AssetDumperSlug::ShouldDump(XAssetInfo<Slug>* asset)
{
    return true;
}

void AssetDumperSlug::DumpAsset(AssetDumpingContext& context, XAssetInfo<Slug>* asset)
{
    const auto* slug = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile)
        return;

    auto& stream = *assetFile;
    stream.write(slug->buffer, slug->len);
}
