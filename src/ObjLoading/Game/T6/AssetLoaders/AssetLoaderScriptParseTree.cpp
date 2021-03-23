#include "AssetLoaderScriptParseTree.h"

#include <cstring>

#include "Game/T6/T6.h"
#include "Pool/GlobalAssetPool.h"

using namespace T6;

void* AssetLoaderScriptParseTree::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* scriptParseTree = memory->Create<ScriptParseTree>();
    memset(scriptParseTree, 0, sizeof(ScriptParseTree));
    scriptParseTree->name = memory->Dup(assetName.c_str());
    return scriptParseTree;
}

bool AssetLoaderScriptParseTree::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderScriptParseTree::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager) const
{
    const auto file = searchPath->Open(assetName);
    if (!file.IsOpen())
        return false;

    auto* scriptParseTree = memory->Create<ScriptParseTree>();
    scriptParseTree->name = memory->Dup(assetName.c_str());
    scriptParseTree->len = static_cast<int>(file.m_length);

    auto* fileBuffer = static_cast<char*>(memory->Alloc(static_cast<size_t>(file.m_length + 1)));
    file.m_stream->read(fileBuffer, file.m_length);
    if (file.m_stream->gcount() != file.m_length)
        return false;
    fileBuffer[scriptParseTree->len] = '\0';

    scriptParseTree->buffer = static_cast<char16*>(fileBuffer);
    manager->AddAsset(ASSET_TYPE_SCRIPTPARSETREE, assetName, scriptParseTree);

    return true;
}
