#include "UiAssets.h"

#include <format>

std::unordered_map<std::string, UiFile> BuildUiFileLookup()
{
    std::unordered_map<std::string, UiFile> result;

    for (const auto& asset : MOD_MAN_UI_FILES)
    {
        result.emplace(std::format("/{}", asset.filename), asset);
    }

    return result;
}
