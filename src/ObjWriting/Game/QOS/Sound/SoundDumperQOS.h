#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/QOS/QOS.h"

namespace sound
{
    class DumperQOS final : public AbstractAssetDumper<QOS::AssetSound>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<QOS::AssetSound::Type>& asset) override;
    };
} // namespace sound
