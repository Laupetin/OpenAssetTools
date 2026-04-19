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

            FontDumpingState(std::string path);
        };

        std::map<const void*, FontDumpingState> m_font_dumping_state_map;

        void CreateFontDumpingState(const void* menuDef, std::string path);
    };
} // namespace font
