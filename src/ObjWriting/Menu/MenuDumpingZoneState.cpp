#include "MenuDumpingZoneState.h"

using namespace menu;

MenuDumpingZoneState::MenuDumpingState::MenuDumpingState(std::string path, const void* aliasMenuList)
    : m_path(std::move(path)),
      m_alias_menu_list(aliasMenuList)
{
}

void MenuDumpingZoneState::CreateMenuDumpingState(const void* menuDef, std::string path, const void* aliasMenuList)
{
    m_menu_dumping_state_map.emplace(std::make_pair(menuDef, MenuDumpingState(std::move(path), aliasMenuList)));
}
