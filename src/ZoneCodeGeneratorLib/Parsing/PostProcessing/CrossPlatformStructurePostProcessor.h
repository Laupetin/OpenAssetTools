#pragma once

#include "IPostProcessor.h"

#include <unordered_set>

class CrossPlatformStructurePostProcessor final : public IPostProcessor
{
public:
    bool PostProcess(IDataRepository* repository) override;
};
