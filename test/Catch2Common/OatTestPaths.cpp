#include "OatTestPaths.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace oat::paths
{
    std::filesystem::path GetSourceDirectory()
    {
        return fs::current_path() / "src";
    }

    std::filesystem::path GetTestDirectory()
    {
        return fs::current_path() / "test";
    }

    std::filesystem::path GetTempDirectory()
    {
        auto result = fs::current_path() / "build" / ".tmp";
        if (!fs::is_directory(result))
            fs::create_directories(result);

        return result;
    }
} // namespace oat::paths
