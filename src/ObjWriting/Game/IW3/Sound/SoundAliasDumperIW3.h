#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace sound_alias
{
    class DumperIW3 final : public AbstractSingleProgressAssetDumper<IW3::AssetSound>
    {
    public:
        void Dump(AssetDumpingContext& context) override;
    };
} // namespace sound_alias
