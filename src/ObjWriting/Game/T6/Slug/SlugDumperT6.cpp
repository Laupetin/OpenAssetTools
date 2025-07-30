#include "SlugDumperT6.h"

using namespace T6;

namespace T6::slug
{
    bool Dumper::ShouldDump(XAssetInfo<Slug>* asset)
    {
        return true;
    }

    void Dumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<Slug>* asset)
    {
        const auto* slug = asset->Asset();
        const auto assetFile = context.OpenAssetFile(asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(slug->buffer, slug->len);
    }
} // namespace T6::slug
