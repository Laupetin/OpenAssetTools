#pragma once

namespace T6
{
    class ObjConstants
    {
        ObjConstants() = default;

    public:
        static constexpr const char* INFO_STRING_PREFIX_PHYS_CONSTRAINTS = "PHYSCONSTRAINTS";
        static constexpr const char* INFO_STRING_PREFIX_PHYS_PRESET = "PHYSIC";
        static constexpr const char* INFO_STRING_PREFIX_TRACER = "TRACER";
        static constexpr const char* INFO_STRING_PREFIX_VEHICLE = "VEHICLEFILE";
        static constexpr const char* INFO_STRING_PREFIX_WEAPON = "WEAPONFILE";
        static constexpr const char* INFO_STRING_PREFIX_WEAPON_ATTACHMENT = "ATTACHMENTFILE";
        static constexpr const char* INFO_STRING_PREFIX_WEAPON_ATTACHMENT_UNIQUE = "ATTACHMENTUNIQUEFILE";
        static constexpr const char* INFO_STRING_PREFIX_ZBARRIER = "ZBARRIER";

        static constexpr const char* GDF_FILENAME_PHYS_CONSTRAINTS = "physconstraints.gdf";
        static constexpr const char* GDF_FILENAME_PHYS_PRESET = "physpreset.gdf";
        static constexpr const char* GDF_FILENAME_TRACER = "tracer.gdf";
        static constexpr const char* GDF_FILENAME_VEHICLE = "vehicle.gdf";
        static constexpr const char* GDF_FILENAME_WEAPON = "weapon.gdf";
        static constexpr const char* GDF_FILENAME_WEAPON_ATTACHMENT = "attachment.gdf";
        static constexpr const char* GDF_FILENAME_WEAPON_ATTACHMENT_UNIQUE = "attachmentunique.gdf";
        static constexpr const char* GDF_FILENAME_ZBARRIER = "zbarrier.gdf";
    };
} // namespace T6
