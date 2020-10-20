#include "AssetDumperRawFile.h"

using namespace T6;

bool AssetDumperRawFile::ShouldDump(XAssetInfo<RawFile>* asset)
{
    return true;
}

std::string AssetDumperRawFile::GetFileNameForAsset(Zone* zone, XAssetInfo<RawFile>* asset)
{
    return asset->m_name;
}

void AssetDumperRawFile::DumpAsset(Zone* zone, XAssetInfo<RawFile>* asset, FileAPI::File* out)
{
    const auto* rawFile = asset->Asset();
    out->Write(rawFile->buffer, 1, rawFile->len);
}