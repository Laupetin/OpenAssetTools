#include <string>

#define DYN_ENT_COUNT 0

// the clipMap->cmodels[0].leaf.terrainContents takes precendence over leaf and material terrain contents

// material flags determine the features of the surface
// unsure which flag type changes what right now
// -1 results in: no running, water splashes all the time, low friction, slanted angles make you slide very fast
// 1 results in: normal surface features, grenades work,
#define MATERIAL_SURFACE_FLAGS 1
#define MATERIAL_CONTENT_FLAGS 1

// terrain flags: does not change the type of terrain or what features they have
// from testing, as long at it isn't 0 things will work correctly
#define LEAF_TERRAIN_CONTENTS 1  // match all flags
#define WORLD_TERRAIN_CONTENTS 1 // match all flags

// const std::string defaultMaterialName = "wpc/intro_wall_plaster_light_blue_01";
const std::string defaultMaterialName = "wpc/dig_plastic_blue";
