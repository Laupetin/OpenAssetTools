#pragma once
#include "Dumping/AssetDumpingContext.h"
#include "Dumping/IZoneAssetDumperState.h"
#include "Weapon/GenericAccuracyGraph.h"

#include <string>
#include <unordered_set>

class AccuracyGraphWriter final : public IZoneAssetDumperState
{
public:
    bool ShouldDumpAiVsAiGraph(const std::string& graphName);
    bool ShouldDumpAiVsPlayerGraph(const std::string& graphName);

    static void DumpAiVsAiGraph(const AssetDumpingContext& context, const GenericAccuracyGraph& aiVsAiGraph);
    static void DumpAiVsPlayerGraph(const AssetDumpingContext& context, const GenericAccuracyGraph& aiVsPlayerGraph);

private:
    std::unordered_set<std::string> m_dumped_ai_vs_ai_graphs;
    std::unordered_set<std::string> m_dumped_ai_vs_player_graphs;
};
