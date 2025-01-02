#pragma once

#include "Asset/IZoneAssetLoaderState.h"
#include "SearchPath/ISearchPath.h"

#include <filesystem>

class IwdCreator : public IZoneAssetLoaderState
{
public:
    void AddFile(std::string filePath);
    void Finalize(ISearchPath& searchPath, const std::filesystem::path& outPath);

private:
    std::vector<std::string> m_file_paths;
};
