#pragma once

#include "IObjCompiler.h"

namespace T5
{
    class ObjCompiler final : public IObjCompiler
    {
    public:
        void ConfigureCreatorCollection(AssetCreatorCollection& collection, Zone& zone, const ZoneDefinition& zoneDefinition) const override;
    };
} // namespace T5
