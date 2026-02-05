#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace techset
{
    class DumperT5 final : public AbstractAssetDumper<T5::AssetTechniqueSet>
    {
    public:
        void Dump(AssetDumpingContext& context) override;

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetTechniqueSet::Type>& asset) override;
    };
} // namespace techset
