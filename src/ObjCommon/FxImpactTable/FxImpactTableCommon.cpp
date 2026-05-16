#include <format>

namespace fx_impact_table
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("fx/{}.csv", assetName);
    }
} // namespace fx_impact_table
