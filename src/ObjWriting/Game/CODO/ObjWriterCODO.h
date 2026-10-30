#pragma once

#include "ObjWriter.h"

namespace CODO
{
    class ObjWriter final : public IObjWriter
    {
    protected:
        void RegisterAssetDumpers(AssetDumpingContext& context) override;
    };
} // namespace CODO
