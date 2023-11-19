#include "AssetDumperRawFile.h"

using namespace T6;

bool AssetDumperRawFile::ShouldDump(XAssetInfo<RawFile>* asset)
{
    return true;
}

void AssetDumperRawFile::DumpAsset(AssetDumpingContext& context, XAssetInfo<RawFile>* asset)
{
    const auto* rawFile = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile)
        return;

    auto& stream = *assetFile;
    stream.write(rawFile->buffer, rawFile->len);
}
