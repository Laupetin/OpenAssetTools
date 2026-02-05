#pragma once

#include "ObjWriter.h"

namespace IW5
{
    class ObjWriter final : public IObjWriter
    {
    public:
        void RegisterAssetDumpers(AssetDumpingContext& context) override;
    };
} // namespace IW5
