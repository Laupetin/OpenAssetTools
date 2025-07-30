#include "StringTableDumperIW4.h"

#include "Csv/CsvStream.h"

using namespace IW4;

namespace IW4::string_table
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
                const auto* cell = &stringTable->values[column + row * stringTable->columnCount];
                if (cell->string != nullptr)
                {
                    csv.WriteColumn(cell->string);
                }
                else
                {
                    csv.WriteColumn("");
                }
            }

            csv.NextRow();
        }
    }
} // namespace IW4::string_table
