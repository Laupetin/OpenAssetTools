#include "AssetDumperScriptParseTree.h"

using namespace T6;

bool AssetDumperScriptParseTree::ShouldDump(XAssetInfo<ScriptParseTree>* asset)
{
    return true;
}

void AssetDumperScriptParseTree::DumpAsset(AssetDumpingContext& context, XAssetInfo<ScriptParseTree>* asset)
{
    const auto* scriptParseTree = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile)
        return;

    auto& stream = *assetFile;
    stream.write(scriptParseTree->buffer, scriptParseTree->len);
}