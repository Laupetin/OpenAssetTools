#pragma once

#include "IPostProcessor.h"

class CreateStructureInformationPostProcessor final : public IPostProcessor
{
public:
    bool PostProcess(IDataRepository* repository) override;
};
