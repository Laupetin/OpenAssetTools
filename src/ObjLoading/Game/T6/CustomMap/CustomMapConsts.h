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

const std::string missingMaterialName = "custom/missing_material";
const std::string colourOnlyMaterialName = "custom/colour_only_material";
const std::string templateMaterialName = "custom/material_template";

#define DEFAULT_SURFACE_LIGHT 1
#define DEFAULT_SURFACE_LIGHTMAP 0
#define DEFAULT_SURFACE_REFLECTION_PROBE 0

#define MAX_AABB_SIZE 512 // maximum size a BSP node can be before it becomes a leaf

// do not change
#define MAX_COL_VERTS (UINT16_MAX - 1) // max amount of verts a map can have

// do not change
#define DEFAULT_LIGHT_INDEX 0
#define SUN_LIGHT_INDEX 1

// do not change
#define SMODEL_FLAG_NO_SHADOW 1
#define SMODEL_FLAG_IS_LIT 2

#define DEFAULT_SMODEL_CULL_DIST 10000.0f
#define DEFAULT_SMODEL_FLAGS SMODEL_FLAG_NO_SHADOW
#define DEFAULT_SMODEL_LIGHT 1
#define DEFAULT_SMODEL_REFLECTION_PROBE 0
