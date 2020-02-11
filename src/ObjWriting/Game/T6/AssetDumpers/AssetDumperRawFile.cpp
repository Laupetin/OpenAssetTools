#include "AssetDumperRawFile.h"

using namespace T6;

bool AssetDumperRawFile::ShouldDump(RawFile* asset)
{
    return true;
}

std::string AssetDumperRawFile::GetFileNameForAsset(Zone* zone, RawFile* asset)
{
    return std::string(asset->name);
}

void AssetDumperRawFile::DumpAsset(Zone* zone, RawFile* asset, FileAPI::File* out)
{
    out->Write(asset->buffer, 1, asset->len);
}