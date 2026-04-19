#pragma once
#include "Dumping/IZoneAssetDumperState.h"

#include <map>

namespace physpreset
{
    class PhysPresetDumpingZoneState final : public IZoneAssetDumperState
    {
    public:
        class PhysPresetDumpingState
        {
        public:
            std::string m_path;

            PhysPresetDumpingState(std::string path);
        };

        std::map<const void*, PhysPresetDumpingState> m_physpreset_dumping_state_map;

        void CreatePhysPresetDumpingState(const void* menuDef, std::string path);
    };
} // namespace menu
