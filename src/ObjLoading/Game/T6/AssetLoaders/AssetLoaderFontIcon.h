#pragma once

#include "AssetLoading/BasicAssetLoader.h"
#include "AssetLoading/IAssetLoadingManager.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"

#include <algorithm>
#include <string>
#include <vector>

namespace T6
{
    class AssetLoaderFontIcon final : public BasicAssetLoader<AssetFontIcon>
    {
        static constexpr unsigned ROW_INDEX = 0;
        static constexpr unsigned ROW_TYPE = 1;

        static constexpr unsigned ROW_ICON_NAME = 2;
        static constexpr unsigned ROW_ICON_MATERIAL = 3;
        static constexpr unsigned ROW_ICON_SIZE = 4;
        static constexpr unsigned ROW_ICON_XSCALE = 5;
        static constexpr unsigned ROW_ICON_YSCALE = 6;

        static constexpr unsigned ROW_ALIAS_NAME = 2;
        static constexpr unsigned ROW_ALIAS_BUTTON = 3;

        static constexpr const char* VALUE_TYPE_ICON = "icon";
        static constexpr const char* VALUE_TYPE_ALIAS = "alias";

        static constexpr unsigned COL_COUNT_ICON = 7;
        static constexpr unsigned COL_COUNT_ALIAS = 4;
        static constexpr unsigned COL_COUNT_MIN = std::min(COL_COUNT_ICON, COL_COUNT_ALIAS);

        static std::string ErrorPrefix(const std::string& assetName, unsigned rowIndex);
        static void PreprocessRow(std::vector<std::string>& row);
        static bool RowIsEmpty(const std::vector<std::string>& row);

        static bool ParseInt(int& value, const std::string& str);
        static bool ParseFloat(float& value, const std::string& str);
        static bool ParseHashStr(int& value, const std::string& str);

        static bool ReadIconRow(const std::vector<std::string>& row,
                                FontIconEntry& icon,
                                const std::string& assetName,
                                unsigned rowIndex,
                                MemoryManager* memory,
                                IAssetLoadingManager* manager,
                                std::vector<XAssetInfoGeneric*>& dependencies);
        static bool ReadAliasRow(const std::vector<std::string>& row,
                                 FontIconAlias& alias,
                                 const std::string& assetName,
                                 unsigned rowIndex,
                                 MemoryManager* memory,
                                 IAssetLoadingManager* manager);

    public:
        _NODISCARD void* CreateEmptyAsset(const std::string& assetName, MemoryManager* memory) override;
        _NODISCARD bool CanLoadFromRaw() const override;
        bool
            LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const override;
    };
} // namespace T6
