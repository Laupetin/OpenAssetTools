#include "AssetDumperRawFile.h"

using namespace T6;

std::string AssetDumperRawFile::GetFileNameForAsset(Zone* zone, RawFile* asset)
{
    return std::string(asset->name);
}

void AssetDumperRawFile::DumpAsset(Zone* zone, RawFile* asset, FileAPI::File* out)
{
    out->Write(asset->buffer, 1, asset->len);
}