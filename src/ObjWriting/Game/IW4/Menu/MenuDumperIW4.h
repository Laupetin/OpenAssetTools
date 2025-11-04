#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace menu
{
    class MenuDumperIW4 final : public AbstractAssetDumper<IW4::AssetMenu>
    {
    public:
        explicit MenuDumperIW4(const AssetPool<IW4::AssetMenu::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetMenu::Type>& asset) override;
    };
} // namespace menu
