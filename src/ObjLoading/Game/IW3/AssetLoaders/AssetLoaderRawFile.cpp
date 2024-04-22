#include "AssetLoaderRawFile.h"

#include "Game/IW3/IW3.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW3;

void* AssetLoaderRawFile::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* rawFile = memory->Create<RawFile>();
    memset(rawFile, 0, sizeof(RawFile));
    rawFile->name = memory->Dup(assetName.c_str());
    return rawFile;
}

bool AssetLoaderRawFile::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderRawFile::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(assetName);
    if (!file.IsOpen())
        return false;

    auto* rawFile = memory->Create<RawFile>();
    rawFile->name = memory->Dup(assetName.c_str());
    rawFile->len = static_cast<int>(file.m_length);

    auto* fileBuffer = memory->Alloc<char>(static_cast<size_t>(file.m_length + 1));
    file.m_stream->read(fileBuffer, file.m_length);
    if (file.m_stream->gcount() != file.m_length)
        return false;
    fileBuffer[rawFile->len] = '\0';

    rawFile->buffer = fileBuffer;
    manager->AddAsset<AssetRawFile>(assetName, rawFile);

    return true;
}
