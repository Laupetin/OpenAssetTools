#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Menu/MenuDumpingZoneState.h"

namespace menu
{
    void CreateDumpingStateForMenuListIW4(MenuDumpingZoneState* zoneState, const IW4::MenuList* menuList);

    class MenuListDumperIW4 final : public AbstractAssetDumper<IW4::AssetMenuList>
    {
    public:
        explicit MenuListDumperIW4(const AssetPool<IW4::AssetMenuList::Type>& pool);

        void Dump(AssetDumpingContext& context) override;

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetMenuList::Type>& asset) override;
    };
} // namespace menu
