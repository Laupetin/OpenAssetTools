#include "OutputPathFilesystem.h"

#include <fstream>

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
    fs::create_directories(containingDirectory);

    std::ofstream stream(fullNewPath, std::ios::binary | std::ios::out);
    if (!stream.is_open())
        return nullptr;

    return std::make_unique<std::ofstream>(std::move(stream));
}
