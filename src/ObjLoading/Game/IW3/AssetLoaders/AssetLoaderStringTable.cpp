#include "AssetLoaderStringTable.h"

#include "Csv/CsvStream.h"
#include "Game/IW3/CommonIW3.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"
#include "StringTable/StringTableLoader.h"

#include <cstring>

using namespace IW3;

void* AssetLoaderStringTable::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* stringTable = memory->Create<StringTable>();
    memset(stringTable, 0, sizeof(StringTable));
    stringTable->name = memory->Dup(assetName.c_str());
    return stringTable;
}

bool AssetLoaderStringTable::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderStringTable::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(assetName);
    if (!file.IsOpen())
        return false;

    string_table::StringTableLoaderV1<StringTable> loader;
    auto* stringTable = loader.LoadFromStream(assetName, *memory, *file.m_stream);

    manager->AddAsset<AssetStringTable>(assetName, stringTable);

    return true;
}
