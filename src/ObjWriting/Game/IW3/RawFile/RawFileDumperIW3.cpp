#include "RawFileDumperIW3.h"

using namespace IW3;

namespace raw_file
{
    bool DumperIW3::ShouldDump(XAssetInfo<RawFile>* asset)
    {
        return true;
    }

    void DumperIW3::DumpAsset(AssetDumpingContext& context, XAssetInfo<RawFile>* asset)
    {
        const auto* rawFile = asset->Asset();
        const auto assetFile = context.OpenAssetFile(asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(rawFile->buffer, rawFile->len);
    }
} // namespace raw_file
