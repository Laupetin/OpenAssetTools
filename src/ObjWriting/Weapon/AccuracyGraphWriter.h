#pragma once
#include "Dumping/AssetDumpingContext.h"
#include "Dumping/IZoneAssetDumperState.h"
#include "Parsing/GenericGraph2D.h"

#include <string>
#include <unordered_set>

class AccuracyGraphWriter final : public IZoneAssetDumperState
{
public:
    bool ShouldDumpAiVsAiGraph(const std::string& graphName);
    bool ShouldDumpAiVsPlayerGraph(const std::string& graphName);

    static void DumpAiVsAiGraph(const AssetDumpingContext& context, const GenericGraph2D& aiVsAiGraph);
    static void DumpAiVsPlayerGraph(const AssetDumpingContext& context, const GenericGraph2D& aiVsPlayerGraph);

private:
    std::unordered_set<std::string> m_dumped_ai_vs_ai_graphs;
    std::unordered_set<std::string> m_dumped_ai_vs_player_graphs;
};
