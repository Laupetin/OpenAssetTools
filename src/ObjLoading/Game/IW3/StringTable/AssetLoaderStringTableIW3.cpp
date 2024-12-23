#include "AssetLoaderStringTableIW3.h"

#include "Game/IW3/IW3.h"
#include "Pool/GlobalAssetPool.h"
#include "StringTable/StringTableLoader.h"

#include <cstring>

using namespace IW3;

AssetLoaderStringTable::AssetLoaderStringTable(MemoryManager& memory, ISearchPath& searchPath)
    : m_memory(memory),
      m_search_path(searchPath)
{
}

AssetCreationResult AssetLoaderStringTable::CreateAsset(const std::string& assetName, AssetCreationContext& context)
{
    const auto file = m_search_path.Open(assetName);
    if (!file.IsOpen())
        return AssetCreationResult::NoAction();

    string_table::StringTableLoaderV1<StringTable> loader;
    auto* stringTable = loader.LoadFromStream(assetName, m_memory, *file.m_stream);
    if (!stringTable)
        return AssetCreationResult::Failure();

    return AssetCreationResult::Success(context.AddAsset<AssetStringTable>(assetName, stringTable));
}
