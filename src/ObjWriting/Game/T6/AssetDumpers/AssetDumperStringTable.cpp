#include "AssetDumperStringTable.h"

#include "Dumping/CsvWriter.h"

using namespace T6;

bool AssetDumperStringTable::ShouldDump(XAssetInfo<StringTable>* asset)
{
    return true;
}

std::string AssetDumperStringTable::GetFileNameForAsset(Zone* zone, XAssetInfo<StringTable>* asset)
{
    return asset->m_name;
}

void AssetDumperStringTable::DumpAsset(AssetDumpingContext& context, XAssetInfo<StringTable>* asset, std::ostream& stream)
{
    const auto* stringTable = asset->Asset();
    CsvWriter csv(stream);

    for(auto row = 0; row < stringTable->rowCount; row++)
    {
        for(auto column = 0; column < stringTable->columnCount; column++)
        {
            const auto* cell = &stringTable->values[column + row * stringTable->columnCount];
            csv.WriteColumn(cell->string);
        }

        csv.NextRow();
    }
}