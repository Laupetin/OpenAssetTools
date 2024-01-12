#include "ObjConstantsT6.h"

namespace T6
{
    const std::array<std::string, 27> ObjConstants::SOUND_GROUPS{
        "grp_reference",
        "grp_master",
        "grp_wpn_lfe",
        "grp_lfe",
        "grp_hdrfx",
        "grp_music",
        "grp_voice",
        "grp_set_piece",
        "grp_igc",
        "grp_mp_game",
        "grp_explosion",
        "grp_player_impacts",
        "grp_scripted_moment",
        "grp_menu",
        "grp_whizby",
        "grp_weapon",
        "grp_vehicle",
        "grp_impacts",
        "grp_foley",
        "grp_destructible",
        "grp_physics",
        "grp_ambience",
        "grp_alerts",
        "grp_air",
        "grp_bink",
        "grp_announcer",
        "",
    };

    const std::array<std::string, 18> ObjConstants::SOUND_CURVES{
        "default",
        "defaultmin",
        "allon",
        "alloff",
        "rcurve0",
        "rcurve1",
        "rcurve2",
        "rcurve3",
        "rcurve4",
        "rcurve5",
        "steep",
        "sindelay",
        "cosdelay",
        "sin",
        "cos",
        "rev60",
        "rev65",
        "",
    };

    const std::array<std::string, 32> ObjConstants::SOUND_DUCK_GROUPS{
        "snp_alerts_gameplay",
        "snp_ambience",
        "snp_claw",
        "snp_destructible",
        "snp_dying",
        "snp_dying_ice",
        "snp_evt_2d",
        "snp_explosion",
        "snp_foley",
        "snp_grenade",
        "snp_hdrfx",
        "snp_igc",
        "snp_impacts",
        "snp_menu",
        "snp_movie",
        "snp_music",
        "snp_never_duck",
        "snp_player_dead",
        "snp_player_impacts",
        "snp_scripted_moment",
        "snp_set_piece",
        "snp_special",
        "snp_vehicle",
        "snp_vehicle_interior",
        "snp_voice",
        "snp_weapon_decay_1p",
        "snp_whizby",
        "snp_wpn_1p",
        "snp_wpn_3p",
        "snp_wpn_turret",
        "snp_x2",
        "snp_x3",
    };

    const std::array<std::string, 4> ObjConstants::SOUND_LIMIT_TYPES{
        "none",
        "oldest",
        "reject",
        "priority",
    };

    const std::array<std::string, 8> ObjConstants::SOUND_MOVE_TYPES{
        "none",
        "left_player",
        "center_player",
        "right_player",
        "random",
        "left_shot",
        "center_shot",
        "right_shot",
    };

    const std::array<std::string, 4> ObjConstants::SOUND_LOAD_TYPES{
        "unknown",
        "loaded",
        "streamed",
        "primed",
    };

    const std::array<std::string, 11> ObjConstants::SOUND_BUS_IDS{
        "bus_reverb",
        "bus_fx",
        "bus_voice",
        "bus_pfutz",
        "bus_hdrfx",
        "bus_ui",
        "bus_reference",
        "bus_music",
        "bus_movie",
        "bus_reference",
        "",
    };

    const std::array<std::string, 4> ObjConstants::SOUND_RANDOMIZE_TYPES{
        "volume",
        "pitch",
        "variant",
        "",
    };

} // namespace T6
