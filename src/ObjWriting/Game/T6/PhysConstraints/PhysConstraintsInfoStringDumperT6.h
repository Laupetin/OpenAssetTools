#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace phys_constraints
{
    class InfoStringDumperT6 final : public AbstractAssetDumper<T6::AssetPhysConstraints>
    {
    public:
        explicit InfoStringDumperT6(const AssetPool<T6::AssetPhysConstraints::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetPhysConstraints::Type>& asset) override;
    };
} // namespace phys_constraints
