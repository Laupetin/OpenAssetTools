#include "AssetDumperScriptParseTree.h"

using namespace T6;

bool AssetDumperScriptParseTree::ShouldDump(ScriptParseTree* asset)
{
    return true;
}

std::string AssetDumperScriptParseTree::GetFileNameForAsset(Zone* zone, ScriptParseTree* asset)
{
    return std::string(asset->name);
}

void AssetDumperScriptParseTree::DumpAsset(Zone* zone, ScriptParseTree* asset, FileAPI::File* out)
{
    out->Write(asset->buffer, 1, asset->len);
}