#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace techset
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetTechniqueSet>
    {
    public:
        void Dump(AssetDumpingContext& context) override;

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetTechniqueSet::Type>& asset) override;
    };
} // namespace techset
