#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace attachment
{
    class JsonDumperIW5 final : public AbstractAssetDumper<IW5::AssetAttachment>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetAttachment::Type>& asset) override;
    };
} // namespace attachment
