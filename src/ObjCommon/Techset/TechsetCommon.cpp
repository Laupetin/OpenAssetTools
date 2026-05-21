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

    size_t CountCountWorldVertFormatParameter(const std::string& assetName, char vertType)
    {
        size_t count = 0;
        if (assetName.empty())
            return count;

        const auto nameLen = assetName.size();
        for (auto pos = 1u; pos < nameLen - 1u; pos++)
        {
            if (assetName[pos - 1] != '_' && !isdigit(assetName[pos - 1]))
                continue;
            if (!isdigit(assetName[pos + 1]))
                continue;

            const auto c = tolower(assetName[pos]);
            if (c == vertType)
                count++;
        }
        return count;
    }

    void CountWorldVertFormatParameters(const std::string& assetName, size_t& texCount, size_t& normalCount, size_t& diffuseCount)
    {
        CountWorldVertFormatParameters(assetName, texCount, normalCount);
        diffuseCount = CountCountWorldVertFormatParameter(assetName, 'd');
    }

    void CountWorldVertFormatParameters(const std::string& assetName, size_t& texCount, size_t& normalCount)
    {
        texCount = CountCountWorldVertFormatParameter(assetName, 'c');
        normalCount = CountCountWorldVertFormatParameter(assetName, 'n');
    }
} // namespace techset
