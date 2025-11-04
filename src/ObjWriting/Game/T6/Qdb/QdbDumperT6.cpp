#include "QdbDumperT6.h"

using namespace T6;

namespace qdb
{
    DumperT6::DumperT6(const AssetPool<AssetQdb::Type>& pool)
        : AbstractAssetDumper(pool)
    {
    }

    void DumperT6::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetQdb::Type>& asset)
    {
        const auto* qdb = asset.Asset();
        const auto assetFile = context.OpenAssetFile(asset.m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(qdb->buffer, qdb->len);
    }
} // namespace qdb
