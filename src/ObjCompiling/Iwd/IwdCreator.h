#pragma once

#include "Asset/IZoneAssetLoaderState.h"
#include "SearchPath/ISearchPath.h"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

class IwdToCreate
{
public:
    explicit IwdToCreate(std::string name);

    void AddFile(std::string filePath);
    void Build(ISearchPath& searchPath, const std::filesystem::path& outPath);

private:
    std::string m_name;
    std::vector<std::string> m_file_paths;
};

class IwdCreator : public IZoneAssetLoaderState
{
public:
    IwdToCreate* GetOrAddIwd(const std::string& iwdName);
    void Finalize(ISearchPath& searchPath, const std::filesystem::path& outPath);

private:
    std::unordered_map<std::string, IwdToCreate*> m_iwd_lookup;
    std::vector<std::unique_ptr<IwdToCreate>> m_iwds;
};
