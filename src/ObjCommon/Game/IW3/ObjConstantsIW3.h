#pragma once

namespace IW3
{
    class ObjConstants
    {
        ObjConstants() = default;

    public:
        static constexpr const char* INFO_STRING_PREFIX_PHYS_PRESET = "PHYSIC";
        static constexpr const char* INFO_STRING_PREFIX_WEAPON = "WEAPONFILE";

        static constexpr const char* GDF_FILENAME_PHYS_PRESET = "physpreset.gdf";
        static constexpr const char* GDF_FILENAME_WEAPON = "weapon.gdf";
    };
} // namespace IW3
