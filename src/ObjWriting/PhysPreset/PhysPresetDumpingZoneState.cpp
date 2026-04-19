#include "FontDumpingZoneState.h"

using namespace font;

FontDumpingZoneState::FontDumpingState::FontDumpingState(std::string path)
    : m_path(std::move(path))
{
}

void FontDumpingZoneState::CreateFontDumpingState(const void* font, std::string path)
{
    m_font_dumping_state_map.emplace(std::make_pair(font, FontDumpingState(std::move(path))));
}
