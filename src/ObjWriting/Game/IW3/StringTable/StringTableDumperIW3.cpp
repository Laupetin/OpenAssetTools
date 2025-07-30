#include "StringTableDumperIW3.h"

#include "Csv/CsvStream.h"

using namespace IW3;

namespace IW3::string_table
{
    bool Dumper::ShouldDump(XAssetInfo<StringTable>* asset)
    {
        return true;
    }

    void Dumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<StringTable>* asset)
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
} // namespace IW3::string_table
