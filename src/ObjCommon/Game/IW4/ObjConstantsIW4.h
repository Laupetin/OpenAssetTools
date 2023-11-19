#pragma once

namespace IW4
{
    class ObjConstants
    {
        ObjConstants() = default;

    public:
        static constexpr const char* INFO_STRING_PREFIX_PHYS_PRESET = "PHYSIC";
        static constexpr const char* INFO_STRING_PREFIX_TRACER = "TRACER";
        static constexpr const char* INFO_STRING_PREFIX_VEHICLE = "VEHICLEFILE";
        static constexpr const char* INFO_STRING_PREFIX_WEAPON = "WEAPONFILE";

        static constexpr const char* GDF_FILENAME_MATERIAL = "material.gdf";
        static constexpr const char* GDF_FILENAME_PHYS_PRESET = "physpreset.gdf";
        static constexpr const char* GDF_FILENAME_TRACER = "tracer.gdf";
        static constexpr const char* GDF_FILENAME_VEHICLE = "vehicle.gdf";
        static constexpr const char* GDF_FILENAME_WEAPON = "weapon.gdf";
    };
} // namespace IW4
