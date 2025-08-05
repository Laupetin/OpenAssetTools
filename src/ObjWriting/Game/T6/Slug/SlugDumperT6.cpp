#include "SlugDumperT6.h"

using namespace T6;

namespace slug
{
    bool DumperT6::ShouldDump(XAssetInfo<Slug>* asset)
    {
        return true;
    }

    void DumperT6::DumpAsset(AssetDumpingContext& context, XAssetInfo<Slug>* asset)
    {
        const auto* slug = asset->Asset();
        const auto assetFile = context.OpenAssetFile(asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(slug->buffer, slug->len);
    }
} // namespace slug
