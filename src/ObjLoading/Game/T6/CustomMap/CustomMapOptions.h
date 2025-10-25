#pragma once

// These options can be edited

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

#define LIGHTGRID_COLOUR 128 // global lighting colour
