#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T4/T4.h"
#include "Menu/MenuDumpingZoneState.h"

namespace menu
{
    void CreateDumpingStateForMenuListT4(MenuDumpingZoneState* zoneState, const T4::MenuList* menuList);

    class MenuListDumperT4 final : public AbstractAssetDumper<T4::AssetMenuList>
    {
    public:
        void Dump(AssetDumpingContext& context) override;

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T4::AssetMenuList::Type>& asset) override;
    };
} // namespace menu
