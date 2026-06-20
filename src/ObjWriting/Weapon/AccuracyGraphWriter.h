#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Dumping/IZoneAssetDumperState.h"
#include "Parsing/GenericGraph2D.h"

#include <string>
#include <unordered_set>

class AccuracyGraphWriter final : public IZoneAssetDumperState
{
public:
    bool ShouldDumpGraph(const std::string& graphName);

    static void DumpGraph(const AssetDumpingContext& context, const GenericGraph2D& graph);

private:
    std::unordered_set<std::string> m_dumped_graphs;
};
