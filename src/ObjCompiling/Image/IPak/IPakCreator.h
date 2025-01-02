#pragma once

#include "Asset/IZoneAssetLoaderState.h"
#include "SearchPath/ISearchPath.h"

#include <filesystem>

class IPakCreator : public IZoneAssetLoaderState
{
public:
    void AddImage(std::string imageName);
    void Finalize(ISearchPath& searchPath, const std::filesystem::path& outPath);

private:
    std::vector<std::string> m_image_names;
};
