#pragma once
#include "AssetLoading/IAssetLoadingManager.h"
#include "AssetLoading/IZoneAssetLoaderState.h"

class PartClassificationState final : public IZoneAssetLoaderState
{
    // TODO: Use MP part classifications when building an mp fastfile
    static constexpr auto PART_CLASSIFICATION_FILE = "partclassification.csv";

    static constexpr auto HITLOC_NONE = 0u;

public:
    PartClassificationState();

    bool Load(const char** hitLocNames, size_t hitLocNameCount, const IAssetLoadingManager& manager);

    [[nodiscard]] unsigned GetPartClassificationForBoneName(const std::string& boneName) const;

private:
    bool LoadRow(const char** hitLocStart, const char** hitLocEnd, unsigned rowIndex, std::vector<std::string>& row);

    bool m_loaded;
    std::unordered_map<std::string, unsigned> m_part_classifications;
};
