#include "AssetDumperRawFile.h"

using namespace T6;

std::string AssetDumperRawFile::GetFileNameForAsset(RawFile* asset)
{
    return std::string(asset->name);
}

void AssetDumperRawFile::DumpAsset(RawFile* asset, FileAPI::File* out)
{
    out->Write(asset->buffer, 1, asset->len);
}