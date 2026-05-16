#pragma once

#include <string_view>
#include <unordered_map>

namespace IW3
{
    enum SndAliasColumn
    {
        COL_NAME,
        COL_SEQUENCE,
        COL_FILE,
        COL_VOL_MIN,
        COL_VOL_MAX,
        COL_VOL_MOD,
        COL_PITCH_MIN,
        COL_PITCH_MAX,
        COL_DIST_MIN,
        COL_DIST_MAX,
        COL_CHANNEL,
        COL_TYPE,
        COL_PROBABILITY,
        COL_LOOP,
        COL_MASTER_SLAVE,
        COL_LOADSPEC,
        COL_SUBTITLE,
        COL_COMPRESSION,
        COL_SECONDARY,
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
                                                "vol_mod",
                                                "pitch_min",
                                                "pitch_max",
                                                "dist_min",
                                                "dist_max",
                                                "channel",
                                                "type",
                                                "probability",
                                                "loop",
                                                "masterslave",
                                                "loadspec",
                                                "subtitle",
                                                "compression",
                                                "secondaryaliasname",
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

    static const char* speaker_map_names[] = {"LEFTSPEAKER", "RIGHTSPEAKER", "CENTERSPEAKER", "LFESPEAKER", "LEFTSURROUNDSPEAKER", "RIGHTSURROUNDSPEAKER"};

    static const char* snd_alias_channel_names[] = {
        "physics", "auto",   "auto2",   "autodog",  "bulletimpact", "bulletwhizby", "element",  "auto2d",       "vehicle",  "vehiclelimited", "menu",
        "body",    "body2d", "reload",  "reload2d", "item",         "effects1",     "effects2", "weapon",       "weapon2d", "nonshock",       "voice",
        "local",   "local2", "ambient", "hurt",     "player1",      "player2",      "music",    "musicnopause", "mission",  "announcer",      "shellshock"};

    static const std::unordered_map<std::string_view, float> volume_mod_groups_table = {
        {{"ac130", 0.8500f},       {"airlift", 0.5400f}, {"aftermath", 0.4500f}, {"ambience", 0.7290f},  {"bodyfall", 0.6529f},  {"chatteral", 0.8100f},
         {"chatterax", 0.8100f},   {"element", 0.9720f}, {"emitter", 0.7290f},   {"explosion", 0.7857f}, {"foley", 0.4860f},     {"footstep", 0.7200f},
         {"footstepplr", 0.4300f}, {"gear", 0.4680f},    {"impact", 1.1250f},    {"impactmp", 1.2150f},  {"interface", 0.8100f}, {"mpfoley", 0.5760f},
         {"mpfootstep", 0.9000f},  {"mpgear", 0.7560f},  {"music", 0.8100f},     {"physics", 0.9000f},   {"quiet", 0.0900f},     {"reactional", 1.0206f},
         {"reactionax", 0.9455f},  {"vehicle", 0.7440f}, {"voiceover", 1.0000f}, {"whizby", 0.7938f},    {"wpnai", 0.7047f},     {"wpnplyr", 0.7695f},
         {"na", 0.8100f},          {"max", 1.0000f}}
    };
} // namespace IW3
