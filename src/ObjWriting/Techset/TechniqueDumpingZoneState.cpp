#include "TechniqueDumpingZoneState.h"

namespace techset
{
    bool TechniqueDumpingZoneState::ShouldDumpTechnique(const void* technique)
    {
        if (m_dumped_techniques.contains(technique))
            return false;

        m_dumped_techniques.emplace(technique);
        return true;
    }
} // namespace techset
