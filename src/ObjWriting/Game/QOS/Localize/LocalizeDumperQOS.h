#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/QOS/QOS.h"

namespace localize
{
    class DumperQOS final : public AbstractSingleProgressAssetDumper<QOS::AssetLocalize>
    {
    public:
        void Dump(AssetDumpingContext& context) override;
    };
} // namespace localize
