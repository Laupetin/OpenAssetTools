#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T4/T4.h"

namespace phys_constraints
{
    class InfoStringDumperT4 final : public AbstractAssetDumper<T4::AssetPhysConstraints>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T4::AssetPhysConstraints::Type>& asset) override;
    };
} // namespace phys_constraints
