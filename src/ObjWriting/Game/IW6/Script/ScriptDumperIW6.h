#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"

namespace script
{
    class DumperIW6 final : public AbstractAssetDumper<IW6::AssetScript>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetScript::Type>& asset) override;
    };
} // namespace script
