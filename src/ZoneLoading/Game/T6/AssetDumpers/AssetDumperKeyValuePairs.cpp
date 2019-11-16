#include "AssetDumperKeyValuePairs.h"

using namespace T6;

std::string AssetDumperKeyValuePairs::GetFileNameForAsset(KeyValuePairs* asset)
{
    return std::string(asset->name);
}

void AssetDumperKeyValuePairs::DumpAsset(KeyValuePairs* asset, FileAPI::File* out)
{
    // TODO
}