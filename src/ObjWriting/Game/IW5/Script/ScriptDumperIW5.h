#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace script
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetScript>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetScript::Type>& asset) override;
    };
} // namespace script
