#include "SndAliasListCommon.h"

#include "Utils/Logging/Log.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace snd_alias_list
{
    const std::string PREFIXES_TO_DROP[]{
        "raw/",
        "devraw/",
    };

    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("soundaliases/{}.csv", assetName);
    }

    std::string GetAssetFilename(const std::string& basePath, const std::string& outputFileName, const std::string& extension)
    {
        fs::path assetPath(basePath);

        std::string tempName = outputFileName;
        std::ranges::replace(tempName, '\\', '/');
        for (const auto& droppedPrefix : PREFIXES_TO_DROP)
        {
            if (tempName.rfind(droppedPrefix, 0) != std::string::npos)
            {
                tempName.erase(0, droppedPrefix.size());
                break;
            }
        }

        assetPath.append(tempName);
        if (!extension.empty())
            assetPath.concat(extension);

        return assetPath.string();
    }

    std::unique_ptr<std::ostream> OpenAssetOutputFile(const std::string& basePath, const std::string& outputFileName, const std::string& extension)
    {
        fs::path assetPath(GetAssetFilename(basePath, outputFileName, extension));

        auto assetDir(assetPath);
        assetDir.remove_filename();

        create_directories(assetDir);

        auto outputStream = std::make_unique<std::ofstream>(assetPath, std::ios_base::out | std::ios_base::binary);

        if (outputStream->is_open())
        {
            return std::move(outputStream);
        }

        return nullptr;
    }

    const char* FromReferencedString(const char* refString)
    {
        if (refString[0] == ',')
        {
            return (refString + 1);
        }
        return refString;
    }

    std::pair<std::string, std::string> SplitPathParts(const std::string& path)
    {
        if (path.empty())
        {
            return {"", ""};
        }

        const char* p = std::strrchr(path.c_str(), '/');
        if (!p)
        {
            return {"", path};
        }

        std::string dir(path.data(), p - path.c_str());
        std::string file(p + 1);
        return {dir, file};
    }

} // namespace snd_alias_list
