#include "FontIconCommonT6.h"

#include "Utils/StringUtils.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace T6::font_icon
{
    std::string GetJsonFileNameForAssetName(const std::string& assetName)
    {
        std::string originalFileName(assetName);
        utils::MakeStringLowerCase(originalFileName);
        if (originalFileName.ends_with(".csv"))
        {
            fs::path p(assetName);
            p.replace_extension(".json");
            return p.string();
        }

        return assetName;
    }
} // namespace T6::font_icon
