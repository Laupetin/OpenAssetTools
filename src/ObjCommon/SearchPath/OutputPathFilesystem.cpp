#include "OutputPathFilesystem.h"

#include "Utils/Logging/Log.h"

#include <format>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

OutputPathFilesystem::OutputPathFilesystem(const fs::path& path)
    : m_path(fs::weakly_canonical(path))
{
}

std::unique_ptr<std::ostream> OutputPathFilesystem::Open(const std::string& fileName)
{
    const auto fullNewPath = fs::weakly_canonical(m_path / fileName);

    if (!fullNewPath.string().starts_with(m_path.string()))
        return nullptr;

    const auto containingDirectory = fullNewPath.parent_path();

    std::error_code ec;
    fs::create_directories(containingDirectory, ec);
    if (ec)
    {
        con::error("Failed to create folder '{}' when try to open file '{}'", containingDirectory.string(), fileName);
        return nullptr;
    }

    std::ofstream stream(fullNewPath, std::ios::binary | std::ios::out);
    if (!stream.is_open())
    {
        con::error("Failed to open file '{}'", fileName);
        return nullptr;
    }

    return std::make_unique<std::ofstream>(std::move(stream));
}
