#pragma once

#include <filesystem>
#include <string>

namespace oat::paths
{
    std::filesystem::path GetSourceDirectory();
    std::filesystem::path GetTestDirectory();
    std::filesystem::path GetTempDirectory();
    std::filesystem::path GetTempDirectory(const std::string& subDir);
} // namespace oat::paths
