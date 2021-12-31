#pragma once
#include <map>

#include "Dumping/IZoneAssetDumperState.h"

namespace menu
{
    class MenuDumpingZoneState final : public IZoneAssetDumperState
    {
    public:
        class MenuDumpingState
        {
        public:
            std::string m_path;
            const void* m_alias_menu_list;

            MenuDumpingState(std::string path, const void* aliasMenuList);
        };

        std::map<const void*, MenuDumpingState> m_menu_dumping_state_map;

        void CreateMenuDumpingState(const void* menuDef, std::string path, const void* aliasMenuList);
    };
}
