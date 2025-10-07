#pragma once

#include "Web/ViteAssets.h"

#include <string>
#include <unordered_map>

namespace ui
{
    std::unordered_map<std::string, UiFile> BuildUiFileLookup();
    const char* GetMimeTypeForFileName(const std::string& fileName);
} // namespace ui
