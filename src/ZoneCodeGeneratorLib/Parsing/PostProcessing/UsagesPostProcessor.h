#pragma once

#include "IPostProcessor.h"

class UsagesPostProcessor final : public IPostProcessor
{
    static bool ProcessAsset(StructureInformation* info);

public:
    bool PostProcess(IDataRepository* repository) override;
};
