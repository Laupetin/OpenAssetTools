#include "AssetLoaderRawFileIW3.h"

#include "Game/IW3/IW3.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW3;

AssetLoaderRawFile::AssetLoaderRawFile(MemoryManager& memory, ISearchPath& searchPath)
    : m_memory(memory),
      m_search_path(searchPath)
{
}

AssetCreationResult AssetLoaderRawFile::CreateAsset(const std::string& assetName, AssetCreationContext& context)
{
    const auto file = m_search_path.Open(assetName);
    if (!file.IsOpen())
        return AssetCreationResult::NoAction();

    auto* rawFile = m_memory.Alloc<RawFile>();
    rawFile->name = m_memory.Dup(assetName.c_str());
    rawFile->len = static_cast<int>(file.m_length);

    auto* fileBuffer = m_memory.Alloc<char>(static_cast<size_t>(file.m_length + 1));
    file.m_stream->read(fileBuffer, file.m_length);
    if (file.m_stream->gcount() != file.m_length)
        return AssetCreationResult::Failure();
    fileBuffer[rawFile->len] = '\0';

    rawFile->buffer = fileBuffer;

    return AssetCreationResult::Success(context.AddAsset<AssetRawFile>(assetName, rawFile));
}
