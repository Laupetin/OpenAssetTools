#include "AssetDumpingContext.h"

#include <filesystem>
#include <format>
#include <fstream>

AssetDumpingContext::AssetDumpingContext()
    : m_zone(nullptr)
{
}

std::unique_ptr<std::ostream> AssetDumpingContext::OpenAssetFile(const std::string& fileName) const
{
    std::filesystem::path assetFilePath(m_base_path);
    assetFilePath.append(fileName);

    auto assetFileFolder(assetFilePath);
    assetFileFolder.replace_filename("");

    std::error_code ec;
    std::filesystem::create_directories(assetFileFolder, ec);

    if (ec)
    {
        std::cerr << std::format("Failed to create folder '{}'. Asset '{}' won't be dumped\n", assetFilePath.string(), fileName);
        return nullptr;
    }

    auto file = std::make_unique<std::ofstream>(assetFilePath, std::fstream::out | std::fstream::binary);

    if (!file->is_open())
    {
        std::cerr << std::format("Failed to open file '{}' to dump asset '{}'\n", assetFilePath.string(), fileName);
        return nullptr;
    }

    return std::move(file);
}
