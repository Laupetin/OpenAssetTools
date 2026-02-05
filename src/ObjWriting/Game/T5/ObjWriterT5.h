#pragma once

#include "ObjWriter.h"

namespace T5
{
    class ObjWriter final : public IObjWriter
    {
    public:
        void RegisterAssetDumpers(AssetDumpingContext& context) override;
    };
} // namespace T5
