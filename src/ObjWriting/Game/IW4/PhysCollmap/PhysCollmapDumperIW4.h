#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace phys_collmap
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetPhysCollMap>
    {
    public:
        explicit DumperIW4(const AssetPool<IW4::AssetPhysCollMap::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetPhysCollMap::Type>& asset) override;
    };
} // namespace phys_collmap
