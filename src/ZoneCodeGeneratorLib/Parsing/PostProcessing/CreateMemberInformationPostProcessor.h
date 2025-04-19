#pragma once

#include "IPostProcessor.h"

class CreateMemberInformationPostProcessor final : public IPostProcessor
{
public:
    bool PostProcess(IDataRepository* repository) override;
};
