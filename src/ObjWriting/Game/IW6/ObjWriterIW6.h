#pragma once

#include "ObjWriter.h"

namespace IW6
{
    class ObjWriter final : public IObjWriter
    {
    public:
        void RegisterAssetDumpers(AssetDumpingContext& context) override;
    };
} // namespace IW6
