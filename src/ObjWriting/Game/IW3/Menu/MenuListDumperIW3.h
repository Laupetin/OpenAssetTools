#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"
#include "Menu/MenuDumpingZoneState.h"

namespace menu
{
    void CreateDumpingStateForMenuListIW3(MenuDumpingZoneState* zoneState, const IW3::MenuList* menuList);

    class MenuListDumperIW3 final : public AbstractAssetDumper<IW3::AssetMenuList>
    {
    public:
        void Dump(AssetDumpingContext& context) override;

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetMenuList::Type>& asset) override;
    };
} // namespace menu
