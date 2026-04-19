#pragma once
#include "Dumping/IZoneAssetDumperState.h"

#include <map>

namespace font
{
    class FontDumpingZoneState final : public IZoneAssetDumperState
    {
    public:
        class FontDumpingState
        {
        public:
            std::string m_path;
            const void* m_alias_menu_list;

            FontDumpingState(std::string path, const void* aliasMenuList);
        };

        std::map<const void*, FontDumpingState> m_font_dumping_state_map;

        void CreateFontDumpingState(const void* menuDef, std::string path, const void* aliasMenuList);
    };
} // namespace menu
