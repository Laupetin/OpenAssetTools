#include "SoundCommon.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace T6::sound
{
    const std::string PREFIXES_TO_DROP[]{
        "raw/",
        "devraw/",
    };

    _NODISCARD std::string GetAssetFilename(const std::string& basePath, std::string outputFileName, const std::string& extension)
    {
        fs::path assetPath(basePath);

        std::replace(outputFileName.begin(), outputFileName.end(), '\\', '/');
        for (const auto& droppedPrefix : PREFIXES_TO_DROP)
        {
            if (outputFileName.rfind(droppedPrefix, 0) != std::string::npos)
            {
                outputFileName.erase(0, droppedPrefix.size());
                break;
            }
        }

        assetPath.append(outputFileName);
        if (!extension.empty())
            assetPath.concat(extension);

        return assetPath.string();
    }
}