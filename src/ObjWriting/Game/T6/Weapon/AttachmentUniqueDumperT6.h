#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace attachment_unique
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetAttachmentUnique>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetAttachmentUnique::Type>& asset) override;
    };
} // namespace attachment_unique
