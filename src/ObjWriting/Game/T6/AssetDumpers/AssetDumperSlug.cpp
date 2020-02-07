#include "AssetDumperSlug.h"

using namespace T6;

std::string AssetDumperSlug::GetFileNameForAsset(Zone* zone, Slug* asset)
{
    return std::string(asset->name);
}

void AssetDumperSlug::DumpAsset(Zone* zone, Slug* asset, FileAPI::File* out)
{
    out->Write(asset->buffer, 1, asset->len);
}