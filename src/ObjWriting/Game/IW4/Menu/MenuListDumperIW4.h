#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/Menu/MenuDumperIW4.h"
#include "Menu/MenuDumpingZoneState.h"

namespace menu
{
    void CreateDumpingStateForMenuListIW4(MenuDumpingZoneState* zoneState, const IW4::MenuList* menuList);

    class MenuListDumperIW4 final : public AbstractAssetDumper<IW4::MenuList>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW4::MenuList>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::MenuList>* asset) override;

    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<IW4::MenuList>* pool) override;
    };
} // namespace menu
