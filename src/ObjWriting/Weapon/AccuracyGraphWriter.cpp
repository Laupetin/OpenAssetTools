#include "AccuracyGraphWriter.h"

#include <format>

namespace
{
    bool ShouldDumpAccuracyGraph(std::unordered_set<std::string>& dumpedGraphs, const std::string& graphName)
    {
        if (graphName.empty())
            return false;

        const auto existingEntry = dumpedGraphs.find(graphName);
        if (existingEntry == dumpedGraphs.end())
        {
            dumpedGraphs.emplace(graphName);
            return true;
        }

        return false;
    }

    void DumpAccuracyGraph(const AssetDumpingContext& context, const GenericAccuracyGraph& graph, const std::string& subFolder)
    {
        const auto file = context.OpenAssetFile(std::format("accuracy/{}/{}", subFolder, graph.name));
        if (!file)
        {
            std::cerr << "Failed to open file for accuracy graph: " << subFolder << "/" << graph.name << "\n";
            return;
        }

        *file << "WEAPONACCUFILE\n\n";
        *file << graph.knots.size() << "\n";

        for (const auto& knot : graph.knots)
            *file << std::format("{:.4f} {:.4f}\n", knot.x, knot.y);
    }
} // namespace

bool AccuracyGraphWriter::ShouldDumpAiVsAiGraph(const std::string& graphName)
{
    return ShouldDumpAccuracyGraph(m_dumped_ai_vs_ai_graphs, graphName);
}

bool AccuracyGraphWriter::ShouldDumpAiVsPlayerGraph(const std::string& graphName)
{
    return ShouldDumpAccuracyGraph(m_dumped_ai_vs_player_graphs, graphName);
}

void AccuracyGraphWriter::DumpAiVsAiGraph(const AssetDumpingContext& context, const GenericAccuracyGraph& aiVsAiGraph)
{
    DumpAccuracyGraph(context, aiVsAiGraph, "aivsai");
}

void AccuracyGraphWriter::DumpAiVsPlayerGraph(const AssetDumpingContext& context, const GenericAccuracyGraph& aiVsPlayerGraph)
{
    DumpAccuracyGraph(context, aiVsPlayerGraph, "aivsplayer");
}
