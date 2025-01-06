#pragma once

#include "IOutputPath.h"

#include <filesystem>

class OutputPathFilesystem final : public IOutputPath
{
public:
    explicit OutputPathFilesystem(const std::filesystem::path& path);

    std::unique_ptr<std::ostream> Open(const std::string& fileName) override;

private:
    std::filesystem::path m_path;
};
