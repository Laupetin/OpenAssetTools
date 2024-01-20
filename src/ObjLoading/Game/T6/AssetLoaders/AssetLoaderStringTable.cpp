#include "AssetLoaderStringTable.h"

#include "Csv/CsvStream.h"
#include "Game/T6/CommonT6.h"
#include "Game/T6/T6.h"
#include "Pool/GlobalAssetPool.h"
#include "StringTable/StringTableLoader.h"

#include <cstring>

using namespace T6;

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

    string_table::StringTableLoaderV3<StringTable, Common::Com_HashString> loader;
    auto* stringTable = loader.LoadFromStream(assetName, *memory, *file.m_stream);

    manager->AddAsset(ASSET_TYPE_STRINGTABLE, assetName, stringTable);

    return true;
}
