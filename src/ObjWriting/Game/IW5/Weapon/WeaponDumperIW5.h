#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace weapon
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetWeapon>
    {
    public:
        explicit DumperIW5(const AssetPool<IW5::AssetWeapon::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetWeapon::Type>& asset) override;
    };
} // namespace weapon
