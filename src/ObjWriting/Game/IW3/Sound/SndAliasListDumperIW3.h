#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace sound_alias_list
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::AssetSound>
    {
    public:
        void Dump(AssetDumpingContext& context) override;

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetSound::Type>& asset) override;
    };
} // namespace sound_alias_list
