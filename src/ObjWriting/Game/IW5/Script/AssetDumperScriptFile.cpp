#include "AssetDumperScriptFile.h"

using namespace IW5;

bool AssetDumperScriptFile::ShouldDump(XAssetInfo<ScriptFile>* asset)
{
    return true;
}

// See https://github.com/xensik/gsc-tool#file-format for an in-depth explanation about the .gscbin format
void AssetDumperScriptFile::DumpAsset(AssetDumpingContext& context, XAssetInfo<ScriptFile>* asset)
{
    auto* scriptFile = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name + ".gscbin");

    if (!assetFile)
        return;

    auto& stream = *assetFile;

    // Dump the name and the numeric fields
    stream.write(asset->m_name.c_str(), asset->m_name.size() + 1);
    stream.write(reinterpret_cast<char*>(&scriptFile->compressedLen), sizeof(scriptFile->compressedLen));
    stream.write(reinterpret_cast<char*>(&scriptFile->len), sizeof(scriptFile->len));
    stream.write(reinterpret_cast<char*>(&scriptFile->bytecodeLen), sizeof(scriptFile->bytecodeLen));

    // Dump the buffers
    stream.write(scriptFile->buffer, scriptFile->compressedLen);
    stream.write(reinterpret_cast<char*>(scriptFile->bytecode), scriptFile->bytecodeLen);
}
