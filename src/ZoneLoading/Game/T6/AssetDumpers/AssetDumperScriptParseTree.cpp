#include "AssetDumperScriptParseTree.h"

using namespace T6;

std::string AssetDumperScriptParseTree::GetFileNameForAsset(ScriptParseTree* asset)
{
    return std::string(asset->name);
}

void AssetDumperScriptParseTree::DumpAsset(ScriptParseTree* asset, FileAPI::File* out)
{
    out->Write(asset->buffer, 1, asset->len);
}