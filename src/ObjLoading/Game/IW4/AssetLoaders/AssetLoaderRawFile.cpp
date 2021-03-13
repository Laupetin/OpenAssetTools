#include "AssetLoaderRawFile.h"

#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

bool AssetLoaderRawFile::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderRawFile::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager) const
{
    const auto file = searchPath->Open(assetName);
    if (!file.IsOpen())
        return false;

    auto* rawFile = memory->Create<RawFile>();
    rawFile->name = memory->Dup(assetName.c_str());
    rawFile->len = static_cast<int>(file.m_length);

    auto* fileBuffer = static_cast<char*>(memory->Alloc(static_cast<size_t>(file.m_length + 1)));
    file.m_stream->read(fileBuffer, file.m_length);
    if (file.m_stream->gcount() != file.m_length)
        return false;

    rawFile->data.buffer = fileBuffer;
    manager->AddAsset(ASSET_TYPE_RAWFILE, assetName, rawFile);

    return true;
}
