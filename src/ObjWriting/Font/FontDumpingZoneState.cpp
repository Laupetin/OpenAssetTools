#include "FontDumpingZoneState.h"

using namespace font;

FontDumpingZoneState::FontDumpingState::FontDumpingState(std::string path, const void* aliasMenuList)
    : m_path(std::move(path)),
      m_alias_menu_list(aliasMenuList)
{
}

void FontDumpingZoneState::CreateFontDumpingState(const void* menuDef, std::string path, const void* aliasMenuList)
{
    m_font_dumping_state_map.emplace(std::make_pair(menuDef, FontDumpingState(std::move(path), aliasMenuList)));
}
