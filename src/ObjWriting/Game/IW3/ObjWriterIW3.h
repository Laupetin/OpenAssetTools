#pragma once

#include "ObjWriter.h"

namespace IW3
{
    class ObjWriter final : public IObjWriter
    {
    protected:
        void RegisterAssetDumpers(AssetDumpingContext& context) override;
    };
} // namespace IW3
