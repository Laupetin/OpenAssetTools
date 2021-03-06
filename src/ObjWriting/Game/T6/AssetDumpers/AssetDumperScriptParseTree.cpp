#include "AssetDumperScriptParseTree.h"

using namespace T6;

bool AssetDumperScriptParseTree::ShouldDump(XAssetInfo<ScriptParseTree>* asset)
{
    return true;
}

bool AssetDumperScriptParseTree::CanDumpAsRaw()
{
    return true;
}

std::string AssetDumperScriptParseTree::GetFileNameForAsset(Zone* zone, XAssetInfo<ScriptParseTree>* asset)
{
    return asset->m_name;
}

void AssetDumperScriptParseTree::DumpRaw(AssetDumpingContext& context, XAssetInfo<ScriptParseTree>* asset, std::ostream& stream)
{
    const auto* scriptParseTree = asset->Asset();
    stream.write(scriptParseTree->buffer, scriptParseTree->len);
}