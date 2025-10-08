#include "ScriptDumperT6.h"

using namespace T6;

namespace script
{
    bool DumperT6::ShouldDump(XAssetInfo<ScriptParseTree>* asset)
    {
        return true;
    }

    void DumperT6::DumpAsset(AssetDumpingContext& context, XAssetInfo<ScriptParseTree>* asset)
    {
        const auto* scriptParseTree = asset->Asset();
        const auto assetFile = context.OpenAssetFile(asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(scriptParseTree->buffer, scriptParseTree->len);
    }
} // namespace script
