#pragma once

#include "Game/IW3/IW3.h"

#include <type_traits>

namespace IW3
{
    inline constexpr auto SND_ALIAS_FLAG_LOOPING = 0x1;
    inline constexpr auto SND_ALIAS_FLAG_MASTER = 0x2;
    inline constexpr auto SND_ALIAS_FLAG_SLAVE = 0x4;
    inline constexpr auto SND_ALIAS_FLAG_FULL_DRY_LEVEL = 0x8;
    inline constexpr auto SND_ALIAS_FLAG_NO_WET_LEVEL = 0x10;
    inline constexpr auto SND_ALIAS_FLAG_RANDOM_LOOPING = 0x20;
    inline constexpr auto SND_ALIAS_FLAG_TYPE_MASK = 0xC0;
    inline constexpr auto SND_ALIAS_FLAG_TYPE_SHIFT = 6;
    inline constexpr auto SND_ALIAS_FLAG_CHANNEL_MASK = 0x3F00;
    inline constexpr auto SND_ALIAS_FLAG_CHANNEL_SHIFT = 8;

    inline constexpr const char* g_pszSndAliasKeyNames[]{
        nullptr,
        "name",
        "sequence",
        "file",
        "subtitle",
        "vol_min",
        "vol_max",
        "vol_mod",
        "pitch_min",
        "pitch_max",
        "dist_min",
        "dist_max",
        "channel",
        "type",
        "loop",
        "probability",
        "loadspec",
        "masterslave",
        "secondaryaliasname",
        "chainaliasname",
        "volumefalloffcurve",
        "startdelay",
        "speakermap",
        "reverb",
        "lfe percentage",
        "center percentage",
        "envelop_min",
        "envelop_max",
        "envelop percentage",
    };
    static_assert(std::extent_v<decltype(g_pszSndAliasKeyNames)> == static_cast<unsigned>(SA_NUMFIELDS));

    inline constexpr const char* SOUND_ALIAS_TYPE_NAMES[]{
        "",
        "loaded",
        "streamed",
    };
    static_assert(std::extent_v<decltype(SOUND_ALIAS_TYPE_NAMES)> == static_cast<unsigned>(SAT_COUNT));

    // Stock channels.def order. The compiled alias stores the index in its flags.
    inline constexpr const char* SOUND_CHANNEL_NAMES[]{
        "physics", "auto",   "auto2",   "autodog",  "bulletimpact", "bulletwhizby", "element",  "auto2d",       "vehicle",  "vehiclelimited", "menu",
        "body",    "body2d", "reload",  "reload2d", "item",         "effects1",     "effects2", "weapon",       "weapon2d", "nonshock",       "voice",
        "local",   "local2", "ambient", "hurt",     "player1",      "player2",      "music",    "musicnopause", "mission",  "announcer",      "shellshock",
    };

    inline constexpr const char* SOUND_SPEAKER_MAP_IDENTIFIERS[]{
        "MONOSOURCE",
        "LEFTSOURCE",
        "RIGHTSOURCE",
        "LEFTSPEAKER",
        "RIGHTSPEAKER",
        "CENTERSPEAKER",
        "LFESPEAKER",
        "LEFTSURROUNDSPEAKER",
        "RIGHTSURROUNDSPEAKER",
    };
    static_assert(std::extent_v<decltype(SOUND_SPEAKER_MAP_IDENTIFIERS)> == static_cast<unsigned>(SA_SPKRMAPIDENTIFIERCOUNT));

    inline constexpr int SOUND_SPEAKER_MAP_IDENTIFIER_VALUES[]{
        0,
        0,
        1,
        0,
        1,
        2,
        3,
        4,
        5,
    };
    static_assert(std::extent_v<decltype(SOUND_SPEAKER_MAP_IDENTIFIER_VALUES)> == static_cast<unsigned>(SA_SPKRMAPIDENTIFIERCOUNT));

    // Stock SDK order plus chainaliasname. Compression and platform are source-tool columns
    // and are therefore not members of snd_alias_members_t.
    inline constexpr const char* SOUND_ALIAS_HEADERS[]{
        g_pszSndAliasKeyNames[SA_NAME],
        g_pszSndAliasKeyNames[SA_SEQUENCE],
        g_pszSndAliasKeyNames[SA_FILE],
        g_pszSndAliasKeyNames[SA_VOL_MIN],
        g_pszSndAliasKeyNames[SA_VOL_MAX],
        g_pszSndAliasKeyNames[SA_VOL_MOD],
        g_pszSndAliasKeyNames[SA_PITCH_MIN],
        g_pszSndAliasKeyNames[SA_PITCH_MAX],
        g_pszSndAliasKeyNames[SA_DIST_MIN],
        g_pszSndAliasKeyNames[SA_DIST_MAX],
        g_pszSndAliasKeyNames[SA_CHANNEL],
        g_pszSndAliasKeyNames[SA_TYPE],
        g_pszSndAliasKeyNames[SA_PROBABILITY],
        g_pszSndAliasKeyNames[SA_LOOP],
        g_pszSndAliasKeyNames[SA_MASTERSLAVE],
        g_pszSndAliasKeyNames[SA_LOADSPEC],
        g_pszSndAliasKeyNames[SA_SUBTITLE],
        "compression",
        g_pszSndAliasKeyNames[SA_SECONDARYALIASNAME],
        g_pszSndAliasKeyNames[SA_VOLUMEFALLOFFCURVE],
        g_pszSndAliasKeyNames[SA_STARTDELAY],
        g_pszSndAliasKeyNames[SA_SPEAKERMAP],
        g_pszSndAliasKeyNames[SA_REVERB],
        g_pszSndAliasKeyNames[SA_LFEPERCENTAGE],
        g_pszSndAliasKeyNames[SA_CENTERPERCENTAGE],
        "platform",
        g_pszSndAliasKeyNames[SA_ENVELOPMIN],
        g_pszSndAliasKeyNames[SA_ENVELOPMAX],
        g_pszSndAliasKeyNames[SA_ENVELOPPERCENTAGE],
        g_pszSndAliasKeyNames[SA_CHAINALIASNAME],
    };
    static_assert(std::extent_v<decltype(SOUND_ALIAS_HEADERS)> == static_cast<unsigned>(SA_NUMFIELDS - SA_NAME) + 2u);
} // namespace IW3
