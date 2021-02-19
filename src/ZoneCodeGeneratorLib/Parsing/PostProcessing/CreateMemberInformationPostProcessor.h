#pragma once

#include "IPostProcessor.h"

class CreateMemberInformationPostProcessor final : public IPostProcessor
{
    bool CreateMemberInformationForStructure(IDataRepository* repository, StructureInformation* structure) const;

public:
    bool PostProcess(IDataRepository* repository) override;
};
