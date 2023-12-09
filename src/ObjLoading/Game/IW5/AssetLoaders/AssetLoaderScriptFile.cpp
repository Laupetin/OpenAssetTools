#include "AssetLoaderScriptFile.h"

#include "Game/IW5/IW5.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <filesystem>
#include <iostream>

using namespace IW5;

void* AssetLoaderScriptFile::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* scriptFile = memory->Create<ScriptFile>();
    memset(scriptFile, 0, sizeof(ScriptFile));
    scriptFile->name = memory->Dup(assetName.c_str());
    return scriptFile;
}

bool AssetLoaderScriptFile::CanLoadFromRaw() const
{
    return true;
}

// See https://github.com/xensik/gsc-tool#file-format for an in-depth explanation about the .gscbin format
bool AssetLoaderScriptFile::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(assetName + ".gscbin");
    if (!file.IsOpen())
        return false;

    const auto fileBuffer = std::make_unique<char[]>(static_cast<size_t>(file.m_length));
    file.m_stream->read(fileBuffer.get(), file.m_length);
    if (file.m_stream->gcount() != file.m_length)
        return false;

    auto* scriptFile = memory->Create<ScriptFile>();
    scriptFile->name = memory->Dup(assetName.c_str());

    // Retrieve data from the buffer
    size_t offset = 0;

    // Read past the name pointer, we will use the one from assetName
    offset += strlen(fileBuffer.get()) + 1;

    memcpy(&scriptFile->compressedLen, fileBuffer.get() + offset, sizeof(scriptFile->compressedLen));
    offset += sizeof(scriptFile->compressedLen);

    memcpy(&scriptFile->len, fileBuffer.get() + offset, sizeof(scriptFile->len));
    offset += sizeof(scriptFile->len);

    memcpy(&scriptFile->bytecodeLen, fileBuffer.get() + offset, sizeof(scriptFile->bytecodeLen));
    offset += sizeof(scriptFile->bytecodeLen);

    // Get the file size
    auto fileSize = file.m_length;

    if (offset + scriptFile->compressedLen > fileSize || offset + scriptFile->bytecodeLen > fileSize)
    {
        std::cerr << "Error: Specified length in " << assetName << " GSC BIN structure exceeds the actual file size" << std::endl;
        return false;
    }

    scriptFile->buffer = static_cast<char*>(memory->Alloc(scriptFile->compressedLen));
    memcpy(const_cast<char*>(scriptFile->buffer), fileBuffer.get() + offset, scriptFile->compressedLen);
    offset += scriptFile->compressedLen;

    scriptFile->bytecode = static_cast<unsigned char*>(memory->Alloc(scriptFile->bytecodeLen));
    memcpy(scriptFile->bytecode, fileBuffer.get() + offset, scriptFile->bytecodeLen);
    offset += scriptFile->bytecodeLen;

    manager->AddAsset(ASSET_TYPE_SCRIPTFILE, assetName, scriptFile);

    return true;
}
