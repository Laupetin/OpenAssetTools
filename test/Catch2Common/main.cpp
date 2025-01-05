#include <catch2/catch_session.hpp>
#include <filesystem>
#include <format>
#include <iostream>

namespace fs = std::filesystem;

int main(const int argc, char* argv[])
{
    const fs::path absoluteBinDir(fs::absolute(argv[0]).parent_path());

    const auto expectedLibDir = absoluteBinDir.parent_path().parent_path();
    const auto expectedBuildDir = expectedLibDir.parent_path();
    const auto expectedRootDir = expectedBuildDir.parent_path();

    if (absoluteBinDir.filename() != "tests" || expectedLibDir.filename() != "lib" || expectedBuildDir.filename() != "build")
    {
        std::cerr << std::format("Expected test binary to be in the folder it was compiled into (build/lib/?/tests) but was {}\n", absoluteBinDir.string());
        std::cerr << "Please do not move test executable out of compilation folder\n";
        return 1;
    }

    const auto expectedSrcDir = expectedRootDir / "src";
    if (!fs::is_directory(expectedSrcDir))
    {
        std::cerr << std::format("Expected source directory to exist in {}, but it did not\n", expectedSrcDir.string());
        std::cerr << "Please do not move test executable out of compilation folder\n";
        return 1;
    }

    const auto expectedTestDir = expectedRootDir / "test";
    if (!fs::is_directory(expectedTestDir))
    {
        std::cerr << std::format("Expected test directory to exist in {}, but it did not\n", expectedTestDir.string());
        std::cerr << "Please do not move test executable out of compilation folder\n";
        return 1;
    }

    fs::current_path(expectedRootDir);

    const auto result = Catch::Session().run(argc, argv);

    const auto tempDir = expectedBuildDir / ".tmp";
    if (fs::is_directory(tempDir))
        fs::remove_all(tempDir);

    return result;
}
