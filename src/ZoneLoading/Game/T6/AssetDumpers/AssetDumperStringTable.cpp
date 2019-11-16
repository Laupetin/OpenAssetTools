#include "AssetDumperStringTable.h"

using namespace T6;

std::string AssetDumperStringTable::GetFileNameForAsset(StringTable* asset)
{
    return std::string(asset->name);
}

void AssetDumperStringTable::DumpAsset(StringTable* asset, FileAPI::File* out)
{
    // TODO
}