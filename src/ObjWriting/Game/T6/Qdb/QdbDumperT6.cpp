#include "QdbDumperT6.h"

using namespace T6;

namespace T6::qdb
{
    bool Dumper::ShouldDump(XAssetInfo<Qdb>* asset)
    {
        return true;
    }

    void Dumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<Qdb>* asset)
    {
        const auto* qdb = asset->Asset();
        const auto assetFile = context.OpenAssetFile(asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(qdb->buffer, qdb->len);
    }
} // namespace T6::qdb
