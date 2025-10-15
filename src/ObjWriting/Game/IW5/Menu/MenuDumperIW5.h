#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace menu
{
    class MenuDumperIW5 final : public AbstractAssetDumper<IW5::AssetMenu>
    {
    public:
        explicit MenuDumperIW5(const AssetPool<IW5::AssetMenu::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetMenu::Type>& asset) override;
    };
} // namespace menu
