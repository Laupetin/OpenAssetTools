#pragma once

#include "IPostProcessor.h"

class CreateTypeInformationPostProcessor final : public IPostProcessor
{
public:
    bool PostProcess(IDataRepository* repository) override;
};
