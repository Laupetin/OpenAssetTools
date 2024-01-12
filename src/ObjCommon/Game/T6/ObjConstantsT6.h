#pragma once
#include <array>
#include <string>

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

        static const std::array<std::string, 27> SOUND_GROUPS;
        static const std::array<std::string, 18> SOUND_CURVES;
        static const std::array<std::string, 32> SOUND_DUCK_GROUPS;
        static const std::array<std::string, 4> SOUND_LIMIT_TYPES;
        static const std::array<std::string, 8> SOUND_MOVE_TYPES;
        static const std::array<std::string, 4> SOUND_LOAD_TYPES;
        static const std::array<std::string, 11> SOUND_BUS_IDS;
        static const std::array<std::string, 4> SOUND_RANDOMIZE_TYPES;
    };
} // namespace T6
