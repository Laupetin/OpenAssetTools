#include "AssetDumperStringTable.h"

#include "Csv/CsvStream.h"

using namespace IW3;

bool AssetDumperStringTable::ShouldDump(XAssetInfo<StringTable>* asset)
{
    return true;
}

void AssetDumperStringTable::DumpAsset(AssetDumpingContext& context, XAssetInfo<StringTable>* asset)
{
    const auto* stringTable = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile)
        return;

    CsvOutputStream csv(*assetFile);

    for (auto row = 0; row < stringTable->rowCount; row++)
    {
        for (auto column = 0; column < stringTable->columnCount; column++)
        {
            csv.WriteColumn(stringTable->values[column + row * stringTable->columnCount]);
        }

        csv.NextRow();
    }
}
