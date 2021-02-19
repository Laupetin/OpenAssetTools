#pragma once

#include "IPostProcessor.h"

class UnionsPostProcessor final : public IPostProcessor
{
    static bool ProcessUnion(StructureInformation* info);

public:
    bool PostProcess(IDataRepository* repository) override;
};
