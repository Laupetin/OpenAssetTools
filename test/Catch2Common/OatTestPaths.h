#pragma once

#include <filesystem>

namespace oat::paths
{
    std::filesystem::path GetSourceDirectory();
    std::filesystem::path GetTestDirectory();
    std::filesystem::path GetTempDirectory();
} // namespace oat::paths
