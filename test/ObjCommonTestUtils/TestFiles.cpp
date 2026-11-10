#include "TestFiles.h"

#include <catch2/catch_test_macros.hpp>
#include <fstream>

namespace fs = std::filesystem;

std::string LoadTestFile(const std::filesystem::path& filePath)
{
    const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

    std::ifstream file(filePath, std::ios::binary);
    REQUIRE(file.is_open());

    std::string data(fileSize, '\0');
    file.read(data.data(), fileSize);

    return data;
}
