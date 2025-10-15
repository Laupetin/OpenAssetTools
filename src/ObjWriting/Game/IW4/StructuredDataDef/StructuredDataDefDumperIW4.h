#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace structured_data_def
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetStructuredDataDef>
    {
    public:
        explicit DumperIW4(const AssetPool<IW4::AssetStructuredDataDef::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetStructuredDataDef::Type>& asset) override;
    };
} // namespace structured_data_def
