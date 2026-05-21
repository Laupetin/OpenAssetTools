#pragma once

#include <string>

namespace techset
{
    std::string GetFileNameForStateMapName(const std::string& stateMapName);
    std::string GetFileNameForTechniqueName(const std::string& assetName);
    std::string GetFileNameForTechsetName(const std::string& assetName);

    size_t CountCountWorldVertFormatParameter(const std::string& assetName, char vertType);

    void CountWorldVertFormatParameters(const std::string& assetName, size_t& texCount, size_t& normalCount);
    void CountWorldVertFormatParameters(const std::string& assetName, size_t& texCount, size_t& normalCount, size_t& diffuseCount);
} // namespace techset
