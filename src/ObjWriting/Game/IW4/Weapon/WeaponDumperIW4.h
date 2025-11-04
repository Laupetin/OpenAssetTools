#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace weapon
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetWeapon>
    {
    public:
        explicit DumperIW4(const AssetPool<IW4::AssetWeapon::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetWeapon::Type>& asset) override;
    };
} // namespace weapon
