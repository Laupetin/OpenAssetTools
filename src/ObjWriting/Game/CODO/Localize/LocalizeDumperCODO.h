#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/CODO/CODO.h"

namespace localize
{
    class DumperCODO final : public AbstractSingleProgressAssetDumper<CODO::AssetLocalize>
    {
    public:
        void Dump(AssetDumpingContext& context) override;
    };
} // namespace localize
