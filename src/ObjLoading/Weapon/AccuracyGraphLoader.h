#pragma once
#include "AssetLoading/IAssetLoadingManager.h"
#include "AssetLoading/IZoneAssetLoaderState.h"
#include "Parsing/GenericGraph2D.h"

#include <memory>

class AccuracyGraphLoader final : public IZoneAssetLoaderState
{
public:
    const GenericGraph2D* LoadAiVsAiGraph(const IAssetLoadingManager* manager, const std::string& graphName);
    const GenericGraph2D* LoadAiVsPlayerGraph(const IAssetLoadingManager* manager, const std::string& graphName);

private:
    std::unordered_map<std::string, std::unique_ptr<GenericGraph2D>> m_loaded_ai_vs_ai_graphs;
    std::unordered_map<std::string, std::unique_ptr<GenericGraph2D>> m_loaded_ai_vs_player_graphs;
};
