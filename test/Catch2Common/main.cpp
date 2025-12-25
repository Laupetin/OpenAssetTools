#include "Utils/Logging/Log.h"

#include <catch2/catch_session.hpp>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>

namespace fs = std::filesystem;

namespace
{
    bool ShouldClearTempFolder()
    {
        auto* envVar = std::getenv("OAT_KEEP_TMP_DIR");

        return !envVar || !envVar[0] || (envVar[0] == '0' && !envVar[1]);
    }
} // namespace

int main(const int argc, char* argv[])
{
    con::init();
    con::set_log_level(con::LogLevel::DEBUG);

    const fs::path absoluteBinDir(fs::canonical(argv[0]).parent_path());

    const auto expectedLibDir = absoluteBinDir.parent_path().parent_path();
    const auto expectedBuildDir = expectedLibDir.parent_path();
    const auto expectedRootDir = expectedBuildDir.parent_path();

    if (absoluteBinDir.filename() != "tests" || expectedLibDir.filename() != "lib" || expectedBuildDir.filename() != "build")
    {
        con::error("Expected test binary to be in the folder it was compiled into (build/lib/?/tests) but was {}", absoluteBinDir.string());
        con::error("Please do not move test executable out of compilation folder");
        return 1;
    }

    const auto expectedSrcDir = expectedRootDir / "src";
    if (!fs::is_directory(expectedSrcDir))
    {
        con::error("Expected source directory to exist in {}, but it did not", expectedSrcDir.string());
        con::error("Please do not move test executable out of compilation folder");
        return 1;
    }

    const auto expectedTestDir = expectedRootDir / "test";
    if (!fs::is_directory(expectedTestDir))
    {
        con::error("Expected test directory to exist in {}, but it did not", expectedTestDir.string());
        con::error("Please do not move test executable out of compilation folder");
        return 1;
    }

    fs::current_path(expectedRootDir);

    const auto result = Catch::Session().run(argc, argv);

    const auto tempDir = expectedBuildDir / ".tmp";
    if (ShouldClearTempFolder())
    {
        con::info("Clearing tmp folder. Define env var OAT_KEEP_TMP_DIR=1 to keep it.");
        if (fs::is_directory(tempDir))
            fs::remove_all(tempDir);
    }
    else
    {
        con::info("Kept tmp dir {} on disk.", tempDir.string());
    }

    return result;
}
