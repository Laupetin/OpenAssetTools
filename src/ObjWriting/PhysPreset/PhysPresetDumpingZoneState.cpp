#include "PhysPresetDumpingZoneState.h"

using namespace physpreset;

PhysPresetDumpingZoneState::PhysPresetDumpingState::PhysPresetDumpingState(std::string path)
    : m_path(std::move(path))
{
}

void PhysPresetDumpingZoneState::CreatePhysPresetDumpingState(const void* physPreset, std::string path)
{
    m_physpreset_dumping_state_map.emplace(std::make_pair(physPreset, PhysPresetDumpingState(std::move(path))));
}
