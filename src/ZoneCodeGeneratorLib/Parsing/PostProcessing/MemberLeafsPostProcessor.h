#pragma once

#include "IPostProcessor.h"

class MemberLeafsPostProcessor final : public IPostProcessor
{
public:
    bool PostProcess(IDataRepository* repository) override;
};
