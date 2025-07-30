#include "RawFileDumperIW3.h"

using namespace IW3;

namespace IW3::raw_file
{
    bool Dumper::ShouldDump(XAssetInfo<RawFile>* asset)
    {
        return true;
    }

    void Dumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<RawFile>* asset)
    {
        const auto* rawFile = asset->Asset();
        const auto assetFile = context.OpenAssetFile(asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(rawFile->buffer, rawFile->len);
    }
} // namespace IW3::raw_file
