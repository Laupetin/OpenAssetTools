#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"

namespace attachment
{
    class JsonDumperIW6 final : public AbstractAssetDumper<IW6::AssetAttachment>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetAttachment::Type>& asset) override;
    };
} // namespace attachment
