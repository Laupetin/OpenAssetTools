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

const char* GetMimeTypeForFileName(const std::string& fileName)
{
    const char* mimeType;

    if (fileName.ends_with(".html"))
        mimeType = "text/html";
    else if (fileName.ends_with(".js"))
        mimeType = "text/javascript";
    else if (fileName.ends_with(".css"))
        mimeType = "text/css";
    else
        mimeType = "application/octet-stream";

    return mimeType;
}
