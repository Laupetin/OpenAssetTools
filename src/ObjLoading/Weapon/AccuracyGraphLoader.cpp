#include "AccuracyGraphLoader.h"

#include "Parsing/Graph2D/Graph2DReader.h"

#include <format>
#include <iostream>

namespace
{
    std::unique_ptr<GenericGraph2D> LoadAccuracyGraph(const IAssetLoadingManager* manager, const std::string& graphName, const std::string& subFolder)
    {
        auto& searchPath = manager->GetAssetLoadingContext()->m_raw_search_path;
        const auto fileName = std::format("accuracy/{}/{}", subFolder, graphName);
        const auto file = searchPath.Open(fileName);
        if (!file.IsOpen())
        {
            std::cerr << std::format("Failed to open file for accuracy graph: {}/{}\n", subFolder, graphName);
            return nullptr;
        }

        return graph2d::Read("accuracy graph", "WEAPONACCUFILE", *file.m_stream, fileName, graphName);
    }
} // namespace

const GenericGraph2D* AccuracyGraphLoader::LoadAiVsAiGraph(const IAssetLoadingManager* manager, const std::string& graphName)
{
    const auto alreadyLoadedGraph = m_loaded_ai_vs_ai_graphs.find(graphName);
    if (alreadyLoadedGraph != m_loaded_ai_vs_ai_graphs.end())
        return alreadyLoadedGraph->second.get();

    auto graph = LoadAccuracyGraph(manager, graphName, "aivsai");
    if (!graph)
        return nullptr;

    const auto* graphPtr = graph.get();
    m_loaded_ai_vs_ai_graphs.emplace(graphName, std::move(graph));

    return graphPtr;
}

const GenericGraph2D* AccuracyGraphLoader::LoadAiVsPlayerGraph(const IAssetLoadingManager* manager, const std::string& graphName)
{
    const auto alreadyLoadedGraph = m_loaded_ai_vs_player_graphs.find(graphName);
    if (alreadyLoadedGraph != m_loaded_ai_vs_player_graphs.end())
        return alreadyLoadedGraph->second.get();

    auto graph = LoadAccuracyGraph(manager, graphName, "aivsplayer");
    if (!graph)
        return nullptr;

    const auto* graphPtr = graph.get();
    m_loaded_ai_vs_player_graphs.emplace(graphName, std::move(graph));

    return graphPtr;
}
