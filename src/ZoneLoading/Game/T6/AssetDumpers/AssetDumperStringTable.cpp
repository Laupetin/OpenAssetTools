#include "AssetDumperStringTable.h"

using namespace T6;

std::string AssetDumperStringTable::GetFileNameForAsset(Zone* zone, StringTable* asset)
{
    return std::string(asset->name);
}

void AssetDumperStringTable::DumpAsset(Zone* zone, StringTable* asset, FileAPI::File* out)
{
    char separator[]{ ',' };
    char newLine[]{ '\n' };

    for(int row = 0; row < asset->rowCount; row++)
    {
        for(int column = 0; column < asset->columnCount; column++)
        {
            const auto cell = &asset->values[column + row * asset->columnCount];

            if (column != 0)
            {
                out->Write(separator, 1, sizeof separator);
            }

            if(cell->string && *cell->string)
            {
                out->Write(cell->string, 1, strlen(cell->string));
            }
        }

        if (row != asset->rowCount - 1)
        {
            out->Write(newLine, 1, sizeof newLine);
        }
    }
}