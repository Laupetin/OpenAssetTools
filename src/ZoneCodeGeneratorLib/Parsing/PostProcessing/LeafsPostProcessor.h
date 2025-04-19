#pragma once

#include "IPostProcessor.h"

class LeafsPostProcessor final : public IPostProcessor
{
public:
    bool PostProcess(IDataRepository* repository) override;
};
