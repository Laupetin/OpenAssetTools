#include "AccuracyGraphWriter.h"

#include "Utils/Logging/Log.h"

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

    void DumpAccuracyGraph(const AssetDumpingContext& context, const GenericGraph2D& graph)
    {
        const auto file = context.OpenAssetFile(std::format("accuracy/{}", graph.name));
        if (!file)
        {
            con::error("Failed to open file for accuracy graph: {}", graph.name);
            return;
        }

        *file << "WEAPONACCUFILE\n\n";
        *file << graph.knots.size() << "\n";

        for (const auto& knot : graph.knots)
            *file << std::format("{:.4f} {:.4f}\n", knot.x, knot.y);
    }
} // namespace

bool AccuracyGraphWriter::ShouldDumpGraph(const std::string& graphName)
{
    return ShouldDumpAccuracyGraph(m_dumped_graphs, graphName);
}

void AccuracyGraphWriter::DumpGraph(const AssetDumpingContext& context, const GenericGraph2D& aiVsAiGraph)
{
    DumpAccuracyGraph(context, aiVsAiGraph);
}
