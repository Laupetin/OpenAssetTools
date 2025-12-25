#include "SystemTestsPaths.h"

#include "OatTestPaths.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace oat::paths
{
    std::filesystem::path GetSystemTestsDirectory()
    {
        return GetTestDirectory() / "SystemTests";
    }
} // namespace oat::paths
