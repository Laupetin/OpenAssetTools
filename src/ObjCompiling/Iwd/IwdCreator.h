#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "SearchPath/IOutputPath.h"
#include "SearchPath/ISearchPath.h"

#include <memory>
#include <string>
#include <unordered_map>

class IwdToCreate
{
public:
    explicit IwdToCreate(std::string name);

    void AddFile(std::string filePath);
    void Build(ISearchPath& searchPath, IOutputPath& outPath);

private:
    std::string m_name;
    std::vector<std::string> m_file_paths;
};

class IwdCreator final : public IZoneAssetCreationState
{
public:
    IwdToCreate* GetOrAddIwd(const std::string& iwdName);
    void Finalize(ISearchPath& searchPath, IOutputPath& outPath);

private:
    std::unordered_map<std::string, IwdToCreate*> m_iwd_lookup;
    std::vector<std::unique_ptr<IwdToCreate>> m_iwds;
};
