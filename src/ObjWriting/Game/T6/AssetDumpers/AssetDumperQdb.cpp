#include "AssetDumperQdb.h"

using namespace T6;

bool AssetDumperQdb::ShouldDump(XAssetInfo<Qdb>* asset)
{
    return true;
}

void AssetDumperQdb::DumpAsset(AssetDumpingContext& context, XAssetInfo<Qdb>* asset)
{
    const auto* qdb = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile)
        return;

    auto& stream = *assetFile;
    stream.write(qdb->buffer, qdb->len);
}
