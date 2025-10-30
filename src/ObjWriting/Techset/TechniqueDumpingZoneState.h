#pragma once

#include "Dumping/IZoneAssetDumperState.h"

#include <unordered_set>

namespace techset
{
    class TechniqueDumpingZoneState final : public IZoneAssetDumperState
    {
    public:
        bool ShouldDumpTechnique(const void* technique);

    private:
        std::unordered_set<const void*> m_dumped_techniques;
    };
} // namespace techset
