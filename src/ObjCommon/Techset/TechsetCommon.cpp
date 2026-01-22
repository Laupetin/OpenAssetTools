#include "TechsetCommon.h"

#include <format>

namespace techset
{
    std::string GetFileNameForStateMapName(const std::string& stateMapName)
    {
        return std::format("statemaps/{}.sm", stateMapName);
    }

    std::string GetFileNameForTechniqueName(const std::string& assetName)
    {
        return std::format("techniques/{}.tech", assetName);
    }

    std::string GetFileNameForTechsetName(const std::string& assetName)
    {
        return std::format("techsets/{}.techset", assetName);
    }

    void CountWorldVertFormatParameters(const std::string& assetName, size_t& texCount, size_t& normalCount)
    {
        texCount = 0;
        normalCount = 0;

        if (assetName.empty())
            return;

        const auto nameLen = assetName.size();
        for (auto pos = 1u; pos < nameLen - 1u; pos++)
        {
            if (assetName[pos - 1] != '_' && !isdigit(assetName[pos - 1]))
                continue;
            if (!isdigit(assetName[pos + 1]))
                continue;

            const auto c = tolower(assetName[pos]);
            if (c == 'c')
                texCount++;
            else if (c == 'n')
                normalCount++;
        }
    }
} // namespace techset
