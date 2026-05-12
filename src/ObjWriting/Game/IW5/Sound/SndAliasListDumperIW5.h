#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace sound_alias_list
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetSound>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetSound::Type>& asset) override;
    };
} // namespace sound_alias_list
