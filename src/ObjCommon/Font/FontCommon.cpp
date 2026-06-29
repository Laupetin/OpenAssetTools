#include "FontCommon.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace font
{
    std::string GetJsonFileNameForAssetName(const std::string& assetName)
    {
        fs::path path(assetName);
        path.replace_extension(".json");
        return path.string();
    }
} // namespace font
