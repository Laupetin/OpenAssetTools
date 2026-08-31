#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"
#include "Menu/MenuDumpingZoneState.h"

namespace menu
{
    void CreateDumpingStateForMenuListIW5(MenuDumpingZoneState* zoneState, const IW5::MenuList* menuList);

    class MenuListDumperIW5 final : public AbstractAssetDumper<IW5::AssetMenuList>
    {
    public:
        void Dump(AssetDumpingContext& context) override;

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetMenuList::Type>& asset) override;
    };
} // namespace menu
