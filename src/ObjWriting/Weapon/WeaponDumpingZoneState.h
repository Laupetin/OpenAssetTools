#pragma once
#include "Dumping/IZoneAssetDumperState.h"

#include <map>

namespace weapon
{
    class WeaponDumpingZoneState final : public IZoneAssetDumperState
    {
    public:
        class WeaponDumpingState
        {
        public:
            std::string m_path;

            WeaponDumpingState(std::string path);
        };

        std::map<const void*, WeaponDumpingState> m_weapon_dumping_state_map;

        void CreateWeaponDumpingState(const void* menuDef, std::string path);
    };
} // namespace weapon
