#include "Utils/Logging/Log.h"

#include <catch2/catch_session.hpp>
#include <filesystem>
#include <format>
#include <iostream>

namespace fs = std::filesystem;

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
    if (fs::is_directory(tempDir))
        fs::remove_all(tempDir);

    return result;
}
