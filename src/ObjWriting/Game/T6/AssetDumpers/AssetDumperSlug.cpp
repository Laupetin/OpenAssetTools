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

void AssetDumperSlug::DumpAsset(Zone* zone, XAssetInfo<Slug>* asset, FileAPI::File* out)
{
    const auto* slug = asset->Asset();
    out->Write(slug->buffer, 1, slug->len);
}