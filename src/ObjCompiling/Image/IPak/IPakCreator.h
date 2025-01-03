#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "SearchPath/ISearchPath.h"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

class IPakToCreate
{
public:
    explicit IPakToCreate(std::string name);

    void AddImage(std::string imageName);
    void Build(ISearchPath& searchPath, const std::filesystem::path& outPath);

private:
    std::string m_name;
    std::vector<std::string> m_image_names;
};

class IPakCreator : public IZoneAssetCreationState
{
public:
    IPakToCreate* GetOrAddIPak(const std::string& ipakName);
    void Finalize(ISearchPath& searchPath, const std::filesystem::path& outPath);

private:
    std::unordered_map<std::string, IPakToCreate*> m_ipak_lookup;
    std::vector<std::unique_ptr<IPakToCreate>> m_ipaks;
};
