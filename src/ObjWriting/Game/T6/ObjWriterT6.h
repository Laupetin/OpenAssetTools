#pragma once
#include "IObjWriter.h"

namespace T6
{
    class ObjWriter final : public IObjWriter
    {
    public:
        bool DumpZone(AssetDumpingContext& context) const override;
    };
} // namespace T6
