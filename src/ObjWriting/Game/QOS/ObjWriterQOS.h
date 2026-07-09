#pragma once

#include "ObjWriter.h"

namespace QOS
{
    class ObjWriter final : public IObjWriter
    {
    protected:
        void RegisterAssetDumpers(AssetDumpingContext& context) override;
    };
} // namespace QOS
