#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "D3DBspReaderIW3.h"

#include <string>
#include <unordered_map>

namespace IW3::d3dbsp
{
    class AssetCreationState final : public IZoneAssetCreationState
    {
    public:
        [[nodiscard]] const LoadResult& Load(const std::string& assetName, ISearchPath& searchPath);

    private:
        std::unordered_map<std::string, LoadResult> m_cache;
    };
} // namespace IW3::d3dbsp
