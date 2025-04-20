#pragma once

#include "IPostProcessor.h"

class CalculateSizeAndAlignPostProcessor final : public IPostProcessor
{
public:
    bool PostProcess(IDataRepository* repository) override;
};
