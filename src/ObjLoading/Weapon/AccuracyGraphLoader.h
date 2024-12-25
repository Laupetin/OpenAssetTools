#pragma once

#include "AssetLoading/IZoneAssetLoaderState.h"
#include "Parsing/GenericGraph2D.h"
#include "SearchPath/ISearchPath.h"

#include <memory>
#include <string>
#include <unordered_map>

class AccuracyGraphLoader final : public IZoneAssetLoaderState
{
public:
    const GenericGraph2D* LoadAiVsAiGraph(ISearchPath& searchPath, const std::string& graphName);
    const GenericGraph2D* LoadAiVsPlayerGraph(ISearchPath& searchPath, const std::string& graphName);

private:
    std::unordered_map<std::string, std::unique_ptr<GenericGraph2D>> m_loaded_ai_vs_ai_graphs;
    std::unordered_map<std::string, std::unique_ptr<GenericGraph2D>> m_loaded_ai_vs_player_graphs;
};
