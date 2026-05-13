#pragma once

#include "Csv/CsvStream.h"

#include <string>
#include <iostream>

namespace fs = std::filesystem;

namespace snd_alias_list
{
    std::string GetFileNameForAssetName(const std::string& assetName);
    std::string GetAssetFilename(const std::string& basePath, const std::string& outputFileName, const std::string& extension);
    std::unique_ptr<std::ostream> OpenAssetOutputFile(const std::string& basePath, const std::string& outputFileName, const std::string& extension);
    const char* FromReferencedString(const char* refString);
    std::pair<std::string, std::string> SplitPathParts(const std::string& path);
} // namespace sound_alias_list
