#pragma once

#include <string>

namespace weapon
{
    std::string GetFileNameForAssetName(const std::string& assetName);

    std::string GetAssetNameForAiVsAiAccuracyGraph(const std::string& graphName);
    std::string GetAssetNameForAiVsPlayerAccuracyGraph(const std::string& graphName);

    std::string GetFileNameForFlameTable(const std::string& flameTableName);
} // namespace weapon
