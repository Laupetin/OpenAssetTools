#pragma once

#include "Game/IW3/IW3.h"

#include <map>
#include <utility>
#include <array>
#include <string_view>
#include <unordered_map>
#include <optional>

namespace IW3
{
    inline cspField_t snd_alias_fields[]{
        {"aliasName",          offsetof(snd_alias_t_info, aliasName),          CSPFT_STRING},
        {"subtitle",           offsetof(snd_alias_t_info, subtitle),           CSPFT_STRING},
        {"secondaryaliasname", offsetof(snd_alias_t_info, secondaryAliasName), CSPFT_STRING},
        {"chainAliasName",     offsetof(snd_alias_t_info, chainAliasName),     CSPFT_STRING},
        {"sequence",           offsetof(snd_alias_t_info, sequence),           CSPFT_INT   },
        {"volMin",             offsetof(snd_alias_t_info, volMin),             CSPFT_FLOAT },
        {"volMax",             offsetof(snd_alias_t_info, volMax),             CSPFT_FLOAT },
        {"pitchMin",           offsetof(snd_alias_t_info, pitchMin),           CSPFT_FLOAT },
        {"pitchMax",           offsetof(snd_alias_t_info, pitchMax),           CSPFT_FLOAT },
        {"distMin",            offsetof(snd_alias_t_info, distMin),            CSPFT_FLOAT },
        {"distMax",            offsetof(snd_alias_t_info, distMax),            CSPFT_FLOAT },
        {"probability",        offsetof(snd_alias_t_info, probability),        CSPFT_FLOAT },
        {"lfePercentage",      offsetof(snd_alias_t_info, lfePercentage),      CSPFT_FLOAT },
        {"centerPercentage",   offsetof(snd_alias_t_info, centerPercentage),   CSPFT_FLOAT },
        {"startDelay",         offsetof(snd_alias_t_info, startDelay),         CSPFT_INT   },
        {"envelopMin",         offsetof(snd_alias_t_info, envelopMin),         CSPFT_FLOAT },
        {"envelopMax",         offsetof(snd_alias_t_info, envelopMax),         CSPFT_FLOAT },
        {"envelopPercentage",  offsetof(snd_alias_t_info, envelopPercentage),  CSPFT_FLOAT },
    };

    static const char* speaker_map_names[] = {
        "LEFTSPEAKER\t\t\t", "RIGHTSPEAKER\t\t", "CENTERSPEAKER\t\t", "LFESPEAKER\t\t\t", "LEFTSURROUNDSPEAKER\t", "RIGHTSURROUNDSPEAKER\t"};

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
