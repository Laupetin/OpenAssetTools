#pragma once
#include "IObjWriter.h"

namespace IW5
{
    class ObjWriter final : public IObjWriter
    {
    public:
        bool DumpZone(AssetDumpingContext& context) const override;
    };
} // namespace IW5
