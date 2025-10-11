#pragma once

#include <string>

// do not change any values in this header file

#define DYN_ENT_COUNT 0

std::string missingImageName = "missing_image";
std::string colorOnlyImageName = "color_only_image";


#define MAX_COL_VERTS (UINT16_MAX - 1) // max amount of collision verts a map can have

#define DEFAULT_LIGHT_INDEX 0
#define SUN_LIGHT_INDEX 1

enum SMODEL_FLAGS
{
    SMODEL_FLAG_NO_SHADOW = 1,
    SMODEL_FLAG_IS_LIT = 2
};

enum GFX_SURFACE_FLAGS
{
    GFX_SURFACE_CASTS_SUN_SHADOW = 0x1,
    GFX_SURFACE_HAS_PRIMARY_LIGHT_CONFLICT = 0x2,
    GFX_SURFACE_IS_SKY = 0x4,
    GFX_SURFACE_NO_DRAW = 0x8,
    GFX_SURFACE_CASTS_SHADOW = 0x10,
    GFX_SURFACE_QUANTIZED = 0x20,
    GFX_SURFACE_NO_COLOR = 0x40
};


#define DEFAULT_SMODEL_CULL_DIST 10000.0f
#define DEFAULT_SMODEL_FLAGS SMODEL_FLAG_NO_SHADOW
#define DEFAULT_SMODEL_LIGHT 1
#define DEFAULT_SMODEL_REFLECTION_PROBE 0

#define DEFAULT_SURFACE_LIGHT SUN_LIGHT_INDEX
#define DEFAULT_SURFACE_LIGHTMAP 0
#define DEFAULT_SURFACE_REFLECTION_PROBE 0
#define DEFAULT_SURFACE_FLAGS (GFX_SURFACE_CASTS_SUN_SHADOW | GFX_SURFACE_CASTS_SHADOW)

const std::vector<std::string> spawnpointDefenderTypeArray = {
    "mp_ctf_spawn_allies",
    "mp_ctf_spawn_allies_start",
    "mp_sd_spawn_defender",
    "mp_dom_spawn_allies_start",
    "mp_dem_spawn_defender_start",
    "mp_dem_spawn_defenderOT_start",
    "mp_dem_spawn_defender",
    "mp_tdm_spawn_allies_start",
    "mp_tdm_spawn_team1_start",
    "mp_tdm_spawn_team2_start",
    "mp_tdm_spawn_team3_start"
};

const std::vector<std::string> spawnpointAttackerTypeArray = {
    "mp_ctf_spawn_axis",
    "mp_ctf_spawn_axis_start",
    "mp_sd_spawn_attacker",
    "mp_dom_spawn_axis_start",
    "mp_dem_spawn_attacker_start",
    "mp_dem_spawn_attackerOT_start",
    "mp_dem_spawn_defender",
    "mp_tdm_spawn_axis_start",
    "mp_tdm_spawn_team4_start",
    "mp_tdm_spawn_team5_start",
    "mp_tdm_spawn_team6_start"
};

const std::vector<std::string> spawnpointFFATypeArray = {
    "mp_tdm_spawn",
    "mp_dm_spawn",
    "mp_dom_spawn"
};

const std::vector<std::string> spawnpointObjectiveTypeArray = {
    "mp_dom_spawn_flag_c",
    "mp_dom_spawn_flag_a"
};

const std::string defaultSpawnpointString = "{"
    "\"attackers\": ["
	"	{"
	"		\"origin\": \"0 0 0\","
	"		\"angles\": \"0 0 0\""
	"	}"
    "],"
	"\"defenders\": ["
	"	{"
	"		\"origin\": \"0 0 0\","
	"		\"angles\": \"0 0 0\""
	"	}"
    "],"
	"\"FFA\": ["
	"	{"
	"		\"origin\": \"0 0 0\","
	"		\"angles\": \"0 0 0\""
	"	}"
	"]"
    "}";


const std::string defaultMapEntsString = "{"
    "\"entities\": ["
        "{"
            "\"classname\": \"worldspawn\""
        "},"
        "{"
            "\"angles\": \"0 0 0\","
            "\"classname\": \"info_player_start\","
            "\"origin\": \"0 0 0\""
        "},"
        "{"
            "\"angles\": \"0 0 0\","
            "\"classname\": \"mp_global_intermission\","
            "\"origin\": \"0 0 0\""
        "}"
    "]"
"}";
