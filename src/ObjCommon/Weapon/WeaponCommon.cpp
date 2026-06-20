#include "WeaponCommon.h"

#include <format>

namespace weapon
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("weapons/{}", assetName);
    }

    std::string GetFileNameForAccuracyGraph(const std::string& assetName)
    {
        return std::format("accuracy/{}", assetName);
    }

    std::string GetAssetNameForAiVsAiAccuracyGraph(const std::string& graphName)
    {
        return std::format("aivsai/{}", graphName);
    }

    std::string GetAssetNameForAiVsPlayerAccuracyGraph(const std::string& graphName)
    {
        return std::format("aivsplayer/{}", graphName);
    }

    std::string GetFileNameForFlameTable(const std::string& flameTableName)
    {
        return std::format("weapons/{}", flameTableName);
    }
} // namespace weapon
