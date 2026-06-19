#pragma once

#include "Dumping/IZoneAssetDumperState.h"

#include <unordered_set>

template<typename T> class SubAssetDeduplicationDumperState final : public IZoneAssetDumperState
{
public:
    [[nodiscard]] bool ShouldDumpSubAsset(const T* subAsset)
    {
        if (m_dumped_sub_assets.contains(subAsset))
            return false;

        m_dumped_sub_assets.emplace(subAsset);
        return true;
    }

private:
    std::unordered_set<const T*> m_dumped_sub_assets;
};
