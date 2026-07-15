#include "OatTestPaths.h"
#include "SearchPath/SearchPathFilesystem.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace
{
    TEST_CASE("SearchPathFilesystem: Does not match a nested filename against a path prefix", "[searchpath]")
    {
        const auto searchPathRoot = oat::paths::GetTempDirectory("SearchPathFilesystem");
        const auto unrelatedFile = searchPathRoot / "iwd" / "maps" / "mp" / "mod.txt";
        fs::create_directories(unrelatedFile.parent_path());
        std::ofstream(unrelatedFile) << "test";

        SearchPathFilesystem searchPath(searchPathRoot.string());
        std::vector<std::string> results;
        searchPath.Find(SearchPathSearchOptions().FilterPrefix("iwd/mod").IncludeSubdirectories(true),
                        [&results](const std::string& path)
                        {
                            results.emplace_back(path);
                        });

        REQUIRE(results.empty());
    }
} // namespace
