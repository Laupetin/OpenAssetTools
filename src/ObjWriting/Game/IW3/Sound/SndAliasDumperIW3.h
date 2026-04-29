#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace snd_alias_list
{
    class SndAliasListDumperIW3 final : public AbstractAssetDumper<IW3::AssetSound>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetSound::Type>& asset) override;
    };
} // namespace snd_alias_list
