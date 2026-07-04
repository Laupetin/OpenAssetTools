#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace phys_constraints
{
    class InfoStringDumperT5 final : public AbstractAssetDumper<T5::AssetPhysConstraints>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetPhysConstraints::Type>& asset) override;
    };
} // namespace phys_constraints
