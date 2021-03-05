#include "AssetDumperQdb.h"

using namespace T6;

bool AssetDumperQdb::ShouldDump(XAssetInfo<Qdb>* asset)
{
    return true;
}

std::string AssetDumperQdb::GetFileNameForAsset(Zone* zone, XAssetInfo<Qdb>* asset)
{
    return asset->m_name;
}

void AssetDumperQdb::DumpAsset(AssetDumpingContext& context, XAssetInfo<Qdb>* asset, std::ostream& stream)
{
    const auto* qdb = asset->Asset();
    stream.write(qdb->buffer, qdb->len);
}