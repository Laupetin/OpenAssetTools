#pragma once


#include <string>

#define DYN_ENT_COUNT 0

// the clipMap->cmodels[0].leaf.terrainContents takes precendence over leaf and material terrain contents

// material flags determine the features of the surface
// unsure which flag type changes what right now
// -1 results in: no running, water splashes all the time, low friction, slanted angles make you slide very fast
// 1 results in: normal surface features, grenades work, seems normal
#define MATERIAL_SURFACE_FLAGS 1
#define MATERIAL_CONTENT_FLAGS 1

// terrain/world flags: does not change the type of terrain or what features they have
// from testing, as long at it isn't 0 things will work correctly
#define LEAF_TERRAIN_CONTENTS 1 
#define WORLD_TERRAIN_CONTENTS 1

std::string missingImageName = "missing_image";
std::string colorOnlyImageName = "color_only_image";

#define MAX_AABB_SIZE 512 // maximum size a BSP node can be before it becomes a leaf

#define LIGHTGRID_COLOUR 128 // global lighting colour

// do not change
#define MAX_COL_VERTS (UINT16_MAX - 1) // max amount of collision verts a map can have

// do not change
#define DEFAULT_LIGHT_INDEX 0
#define SUN_LIGHT_INDEX 1

// do not change
enum SMODEL_FLAGS
{
    SMODEL_FLAG_NO_SHADOW = 1,
    SMODEL_FLAG_IS_LIT = 2
};

// do not change
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

#define DEFAULT_SURFACE_LIGHT 1
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


