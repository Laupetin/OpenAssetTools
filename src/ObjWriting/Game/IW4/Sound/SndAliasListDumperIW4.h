#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace sound_alias_list
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetSound>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetSound::Type>& asset) override;
    };
} // namespace sound_alias_list
