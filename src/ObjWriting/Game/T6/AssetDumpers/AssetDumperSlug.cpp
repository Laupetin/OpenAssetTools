#include "AssetDumperSlug.h"

using namespace T6;

bool AssetDumperSlug::ShouldDump(XAssetInfo<Slug>* asset)
{
    return true;
}

std::string AssetDumperSlug::GetFileNameForAsset(Zone* zone, XAssetInfo<Slug>* asset)
{
    return asset->m_name;
}

void AssetDumperSlug::DumpAsset(Zone* zone, XAssetInfo<Slug>* asset, std::ostream& stream)
{
    const auto* slug = asset->Asset();
    stream.write(slug->buffer, slug->len);
}