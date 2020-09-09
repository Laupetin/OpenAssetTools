#include "AssetDumperStringTable.h"

#include "Dumping/CsvWriter.h"

using namespace IW4;

bool AssetDumperStringTable::ShouldDump(StringTable* asset)
{
    return true;
}

std::string AssetDumperStringTable::GetFileNameForAsset(Zone* zone, StringTable* asset)
{
    return std::string(asset->name);
}

void AssetDumperStringTable::DumpAsset(Zone* zone, StringTable* asset, FileAPI::File* out)
{
    CsvWriter csv(out);

    for (int row = 0; row < asset->rowCount; row++)
    {
        for (int column = 0; column < asset->columnCount; column++)
        {
            const auto* cell = &asset->values[column + row * asset->columnCount];
            csv.WriteColumn(cell->string);
        }

        csv.NextRow();
    }
}