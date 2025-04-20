#pragma once

#include "IPostProcessor.h"

#include <unordered_set>

class MarkingRequiredPostProcessor final : public IPostProcessor
{
public:
    bool PostProcess(IDataRepository* repository) override;
};
