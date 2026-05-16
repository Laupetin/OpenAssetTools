#pragma once

#include <string_view>
#include <unordered_map>

namespace IW5
{
    enum SndAliasColumn
    {
        COL_NAME,
        COL_SEQUENCE,
        COL_FILE,
        COL_VOL_MIN,
        COL_VOL_MAX,
        COL_VOL_MOD_INDEX,
        COL_PITCH_MIN,
        COL_PITCH_MAX,
        COL_DIST_MIN,
        COL_DIST_MAX,
        COL_VELOCITY_MIN,
        COL_CHANNEL,
        COL_TYPE,
        COL_PROBABILITY,
        COL_LOOP,
        COL_MASTER_PRIORITY,
        COL_MASTER_SLAVE,
        COL_LOADSPEC,
        COL_SUBTITLE,
        COL_COMPRESSION,
        COL_SECONDARY,
        COL_MIXER_GROUP,
        COL_FALLOFF,
        COL_START_DELAY,
        COL_SPEAKERMAP,
        COL_REVERB,
        COL_LFE,
        COL_CENTER,
        COL_PLATFORM,
        COL_ENV_MIN,
        COL_ENV_MAX,
        COL_ENV_PERCENT,
        COL_CONVERSION
    };

    static const char* sound_alias_headers[] = {"name",
                                                "sequence",
                                                "file",
                                                "vol_min",
                                                "vol_max",
                                                "vol_mod_index",
                                                "pitch_min",
                                                "pitch_max",
                                                "dist_min",
                                                "dist_max",
                                                "velocity_min",
                                                "channel",
                                                "type",
                                                "probability",
                                                "loop",
                                                "masterpriority",
                                                "masterslave",
                                                "loadspec",
                                                "subtitle",
                                                "compression",
                                                "secondaryaliasname",
                                                "mixergroup",
                                                "volumefalloffcurve",
                                                "startdelay",
                                                "speakermap",
                                                "reverb",
                                                "lfe percentage",
                                                "center percentage",
                                                "platform",
                                                "envelop_min",
                                                "envelop_max",
                                                "envelop percentage",
                                                "conversion"};
    static const char* speaker_map_names[] = {
        "LEFTSPEAKER\t\t\t", "RIGHTSPEAKER\t\t", "CENTERSPEAKER\t\t", "LFESPEAKER\t\t\t", "LEFTSURROUNDSPEAKER\t", "RIGHTSURROUNDSPEAKER\t"};

    static const char* snd_alias_channel_names[] = {
        "physics",         "ambdist1",   "ambdist2",     "alarm",       "auto2",        "auto2d",       "autodog",      "explosiondist1", "explosiondist2",
        "explosiveimpact", "element",    "element_int",  "element_ext", "bulletimpact", "bulletflesh1", "bulletflesh2", "bulletwhizby",   "vehicle",
        "vehiclelimited",  "menu",       "body",         "body2d",      "reload",       "reload2d",     "item",         "explosion1",     "explosion2",
        "explosion3",      "explosion4", "explosion5",   "effects1",    "effects2",     "effects3",     "effects2d1",   "effects2d2",     "norestrict",
        "norestrict2d",    "aircraft",   "vehicle2d",    "weapon_dist", "weapon_mid",   "weapon",       "weapon2d",     "nonshock",       "nonshock2",
        "grondo3d",        "grondo2d",   "voice",        "local",       "local2",       "local3",       "ambient",      "hurt",           "player1",
        "player2",         "music",      "musicnopause", "mission",     "critical",     "announcer",    "shellshock"};

    static const std::unordered_map<std::string_view, float> volume_mod_groups_table = {
        // OLD / LEGACY
        {"foley",               0.545f },
        {"wpnai",               0.8228f},
        {"wpnplyr",             0.704f },

        // User interface
        {"hud",                 0.7533f},
        {"interface",           0.7533f},
        {"interface_music",     0.7533f},

        // Music
        {"music",               0.81f  },
        {"music_emitter",       0.81f  },

        // Ambience
        {"ambience",            0.678f },
        {"ambience_dist",       0.904f },
        {"element",             0.904f },
        {"emitter",             0.755f },
        {"physics",             0.837f },

        // Character sounds
        {"bodyfall",            0.7002f},
        {"foley_plr",           0.545f },
        {"foleymp_plr",         0.7031f},
        {"foley_npc",           0.6582f},
        {"foleymp_npc",         0.5357f},
        {"foley_wpn_plr",       0.865f },
        {"foley_wpn_npc",       0.84f  },
        {"footstep_plr",        0.6229f},
        {"footstep_npc",        0.8126f},
        {"footstepmp_plr",      0.6229f},
        {"footstepmp_npc",      0.837f },
        {"melee_plr",           0.7533f},
        {"melee_npc",           0.7533f},

        // Dialogue
        {"chatteral",           0.9f   },
        {"chatterax",           0.85f  },
        {"reactional",          0.9492f},
        {"reactionax",          0.8979f},
        {"voiceover",           1.0f   },
        {"voiceover_radio",     1.0f   },
        {"voiceover_critical",  1.0f   },
        {"voiceover_amb",       1.0f   },

        // Explosions / destruction
        {"destruct",            0.7807f},
        {"explosion",           0.7807f},
        {"explosion_grenade",   0.85f  },
        {"explosion_flashbang", 0.7807f},
        {"explosion_rocket",    0.85f  },
        {"explosion_car",       0.85f  },

        // Impacts / whizbys
        {"impact",              0.93f  },
        {"impact_plr",          0.7533f},
        {"impact_npc",          0.65f  },
        {"impactmp",            0.93f  },
        {"impactmp_plr",        0.7533f},
        {"impactmp_npc",        0.7533f},
        {"whizby",              0.8312f},
        {"whizbymp",            0.8312f},

        // Vehicles
        {"vehicle_plr",         0.6919f},
        {"vehicle_npc",         0.6919f},
        {"vehicle_wpn_plr",     0.704f },
        {"vehicle_wpn_npc",     0.8228f},
        {"vehicle",             0.6919f},

        // Weapons
        {"grenadebounce",       0.7533f},
        {"grenadebouncemp",     0.7533f},
        {"shellcasings",        0.93f  },
        {"shellcasingsmp",      0.93f  },
        {"wpn_plr",             0.85f  },
        {"wpnmp_plr",           0.85f  },
        {"wpn_npc",             0.87f  },
        {"wpnmp_npc",           0.87f  },
        {"wpn_projectile",      0.9f   },
        {"wpnmp_projectile",    0.93f  },

        // Special use
        {"na",                  0.7533f},
        {"max",                 0.93f  },
        {"scripted1",           0.93f  },
        {"scripted2",           0.93f  },
        {"scripted3",           0.93f  },
        {"scripted4",           0.93f  },
        {"scripted5",           0.93f  },
        {"fullvolume",          1.0f   },

        // Level specific / perks
        {"perkmp_quiet",        0.0837f},
        {"level_ac130",         0.7905f},

        // Default
        {"default",             1.0f   },
    };
} // namespace IW5
