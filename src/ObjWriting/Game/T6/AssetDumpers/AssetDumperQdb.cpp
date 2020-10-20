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

void AssetDumperQdb::DumpAsset(Zone* zone, XAssetInfo<Qdb>* asset, FileAPI::File* out)
{
    const auto* qdb = asset->Asset();
    out->Write(qdb->buffer, 1, qdb->len);
}