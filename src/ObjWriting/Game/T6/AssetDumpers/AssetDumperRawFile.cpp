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

void AssetDumperRawFile::DumpAsset(Zone* zone, XAssetInfo<RawFile>* asset, std::ostream& stream)
{
    const auto* rawFile = asset->Asset();
    stream.write(rawFile->buffer, rawFile->len);
}