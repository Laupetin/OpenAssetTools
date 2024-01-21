#pragma once

#include "IPostProcessor.h"

#include <unordered_set>

class MarkingRequiredPostProcessor final : public IPostProcessor
{
    static bool RequiresMarking(std::unordered_set<const void*>& visitedStructures, const StructureInformation* info);

public:
    bool PostProcess(IDataRepository* repository) override;
};
