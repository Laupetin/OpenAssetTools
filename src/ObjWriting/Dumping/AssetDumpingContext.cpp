#include "AssetDumpingContext.h"

#include <filesystem>
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
    create_directories(assetFileFolder);

    auto file = std::make_unique<std::ofstream>(assetFilePath, std::fstream::out | std::fstream::binary);

    if (!file->is_open())
    {
        std::cout << "Failed to open file '" << assetFilePath.string() << "' to dump asset '" << fileName << "'\n";
        return nullptr;
    }

    return std::move(file);
}
