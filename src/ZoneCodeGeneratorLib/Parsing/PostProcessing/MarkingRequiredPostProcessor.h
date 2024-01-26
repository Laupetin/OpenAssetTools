#pragma once

#include "IPostProcessor.h"

#include <unordered_set>

class MarkingRequiredPostProcessor final : public IPostProcessor
{
    static bool CalculateRequiresMarking(std::unordered_set<const void*>& visitedStructures, StructureInformation* info);

public:
    bool PostProcess(IDataRepository* repository) override;
};
