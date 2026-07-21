#include "D3DBspAssetCreationStateIW3.h"

#include <utility>

using namespace IW3::d3dbsp;

const LoadResult& AssetCreationState::Load(const std::string& assetName, ISearchPath& searchPath)
{
    const auto existingEntry = m_cache.find(assetName);
    if (existingEntry != m_cache.end())
        return existingEntry->second;

    auto result = LoadFromSearchPath(assetName, searchPath);
    const auto insertResult = m_cache.emplace(assetName, std::move(result));
    return insertResult.first->second;
}
