#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace phys_constraints
{
    class InfoStringDumperT6 final : public AbstractAssetDumper<T6::PhysConstraints>
    {
    protected:
        bool ShouldDump(XAssetInfo<T6::PhysConstraints>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::PhysConstraints>* asset) override;
    };
} // namespace phys_constraints
