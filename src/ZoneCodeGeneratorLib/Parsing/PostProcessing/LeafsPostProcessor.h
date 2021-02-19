#pragma once

#include "IPostProcessor.h"

class LeafsPostProcessor final : public IPostProcessor
{
    static bool IsLeaf(StructureInformation* info);

public:
    bool PostProcess(IDataRepository* repository) override;
};
