#include "WeaponDumpingZoneState.h"

using namespace weapon;

WeaponDumpingZoneState::WeaponDumpingState::WeaponDumpingState(std::string path)
    : m_path(std::move(path))
{
}

void WeaponDumpingZoneState::CreateWeaponDumpingState(const void* physPreset, std::string path)
{
    m_weapon_dumping_state_map.emplace(std::make_pair(physPreset, WeaponDumpingState(std::move(path))));
}
