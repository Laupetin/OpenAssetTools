#pragma once

#include "IPostProcessor.h"

class UnionsPostProcessor final : public IPostProcessor
{
public:
    bool PostProcess(IDataRepository* repository) override;
};
