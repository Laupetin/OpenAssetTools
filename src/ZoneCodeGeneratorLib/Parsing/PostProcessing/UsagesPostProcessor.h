#pragma once

#include "IPostProcessor.h"

class UsagesPostProcessor final : public IPostProcessor
{
public:
    bool PostProcess(IDataRepository* repository) override;
};
