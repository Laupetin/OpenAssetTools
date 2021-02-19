#pragma once

#include "IPostProcessor.h"

class MemberLeafsPostProcessor final : public IPostProcessor
{
    static bool MemberIsLeaf(MemberInformation* member);
    static void ProcessInfo(StructureInformation* info);

public:
    bool PostProcess(IDataRepository* repository) override;
};
