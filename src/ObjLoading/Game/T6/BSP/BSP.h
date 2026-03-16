#pragma once

#include "Game/T6/T6.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <memory>
#include <string>
#include <vector>
using namespace T6;

namespace BSP
{
    enum BSPMaterialType
    {
        MATERIAL_TYPE_COLOUR,
        MATERIAL_TYPE_TEXTURE
    };

    struct BSPVertex
    {
        vec3_t pos;
        vec4_t color;
        vec2_t texCoord;
        vec3_t normal;
        vec3_t tangent;
        vec3_t binormal;
    };

    struct BSPMaterial
    {
        std::string materialName;
        BSPMaterialType materialType;
        vec4_t materialColour;
    };

    struct BSPSurface
    {
        size_t materialIndex;
        uint16_t vertexCount;
        uint16_t triCount;
        int indexOfFirstVertex;
        int indexOfFirstIndex;
    };

    struct BSPWorld
    {
        std::vector<BSPSurface> surfaces;
        std::vector<BSPVertex> vertices;
        std::vector<uint16_t> indices;
        std::vector<BSPMaterial> materials;
    };

    enum BSPLightType
    {
        LIGHT_TYPE_DIRECTIONAL,
        LIGHT_TYPE_POINT,
        LIGHT_TYPE_SPOT
    };

    struct BSPLight
    {
        BSPLightType type;
        vec3_t colour;
        float range;
        float intensity;

        vec3_t pos;
        vec3_t direction;
        bool hasPosBeenSet;

        // angle is in radians. only used on spot/dir lights
        float innerConeAngle;
        float outerConeAngle;
    };

    struct BSPData
    {
        std::string name;
        std::string bspName;

        BSPWorld gfxWorld;
        BSPWorld colWorld;

        std::vector<BSPLight> lights;
    };

    // BSPGameConstants:
    // These values are hardcoded ingame and will break the map if they are changed
    namespace BSPGameConstants
    {
        constexpr unsigned int MAX_COLLISION_VERTS = UINT16_MAX;

        constexpr size_t MAX_AABB_TREE_CHILDREN = 128;

        enum BSPDefaultLights
        {
            EMPTY_LIGHT_INDEX = 0,
            SUN_LIGHT_INDEX = 1,
            BSP_DEFAULT_LIGHT_COUNT = 2
        };

        inline const char* DEFENDER_SPAWN_POINT_NAMES[] = {"mp_ctf_spawn_allies",
                                                           "mp_ctf_spawn_allies_start",
                                                           "mp_sd_spawn_defender",
                                                           "mp_dom_spawn_allies_start",
                                                           "mp_dem_spawn_defender_start",
                                                           "mp_dem_spawn_defenderOT_start",
                                                           "mp_dem_spawn_defender",
                                                           "mp_tdm_spawn_allies_start",
                                                           "mp_tdm_spawn_team1_start",
                                                           "mp_tdm_spawn_team2_start",
                                                           "mp_tdm_spawn_team3_start"};

        inline const char* ATTACKER_SPAWN_POINT_NAMES[] = {"mp_ctf_spawn_axis",
                                                           "mp_ctf_spawn_axis_start",
                                                           "mp_sd_spawn_attacker",
                                                           "mp_dom_spawn_axis_start",
                                                           "mp_dem_spawn_attacker_start",
                                                           "mp_dem_spawn_attackerOT_start",
                                                           "mp_dem_spawn_defender",
                                                           "mp_tdm_spawn_axis_start",
                                                           "mp_tdm_spawn_team4_start",
                                                           "mp_tdm_spawn_team5_start",
                                                           "mp_tdm_spawn_team6_start"};

        inline const char* FFA_SPAWN_POINT_NAMES[] = {"mp_tdm_spawn", "mp_dm_spawn", "mp_dom_spawn"};
    } // namespace BSPGameConstants

    // BSPLinkingConstants:
    // These values are BSP linking constants that are required for the link to be successful
    namespace BSPLinkingConstants
    {
        constexpr const char* MISSING_IMAGE_NAME = ",mc/lambert1";
        constexpr const char* COLOR_ONLY_IMAGE_NAME = ",mc/lambert1";

        constexpr const char* DEFAULT_SPAWN_POINT_STRING = R"({
    "attackers": [
		{
			"origin": "0 0 0",
			"angles": "0 0 0"
		}
    ],
	"defenders": [
		{
			"origin": "0 0 0",
			"angles": "0 0 0"
		}
    ],
	"FFA": [
		{
			"origin": "0 0 0",
			"angles": "0 0 0"
		}
	]
    })";

        constexpr const char* DEFAULT_MAP_ENTS_STRING = R"({
    "entities": [
        {
            "classname": "worldspawn"
        },
        {
            "angles": "0 0 0",
            "classname": "info_player_start",
            "origin": "0 0 0"
        },
        {
            "angles": "0 0 0",
            "classname": "mp_global_intermission",
            "origin": "0 0 0"
        }
    ]
    })";
    } // namespace BSPLinkingConstants

    // BSPEditableConstants:
    // These values are BSP constants that can be edited and may not break the linker/game if changed
    namespace BSPEditableConstants
    {
        // Default xmodel values
        // Unused as there is no support for xmodels right now
        constexpr float DEFAULT_SMODEL_CULL_DIST = 10000.0f;
        constexpr int DEFAULT_SMODEL_FLAGS = STATIC_MODEL_FLAG_NO_SHADOW;
        constexpr int DEFAULT_SMODEL_LIGHT = 1;
        constexpr int DEFAULT_SMODEL_REFLECTION_PROBE = 0;

        // Default surface values
        constexpr int DEFAULT_SURFACE_LIGHT = 1;
        constexpr int DEFAULT_SURFACE_LIGHTMAP = 0;
        constexpr int DEFAULT_SURFACE_REFLECTION_PROBE = 0;
        constexpr int DEFAULT_SURFACE_FLAGS = 0;

        // material flags determine the features of a surface
        // unsure which flag type changes what right now
        // -1 results in: no running, water splashes all the time, low friction, slanted angles make you slide very fast
        // 1 results in: normal surface features, grenades work, seems normal
        constexpr int MATERIAL_SURFACE_FLAGS = 1;
        constexpr int MATERIAL_CONTENT_FLAGS = 1;

        // terrain/world flags: does not change the type of terrain or what features they have
        // from testing, as long at it isn't 0 things will work correctly
        constexpr int LEAF_TERRAIN_CONTENTS = 1;
        constexpr int WORLD_TERRAIN_CONTENTS = 1;

        // lightgrid (global) lighting colour
        // since lightgrids are not well understood, this colour is used for the R, G and B values right now
        constexpr unsigned char LIGHTGRID_COLOUR = 128;

        // Sunlight values
        constexpr vec4_t SUNLIGHT_COLOR = {0.75f, 0.75f, 0.75f, 1.0f};
        constexpr vec3_t SUNLIGHT_DIRECTION = {0.0f, 0.0f, 0.0f};
    }; // namespace BSPEditableConstants

    namespace BSPFlags
    {
        // enum SurfaceType
        //{
        //     SURF_TYPE_BARK,
        //     SURF_TYPE_BRICK,
        //     SURF_TYPE_CARPET,
        //     SURF_TYPE_CLOTH,
        //     SURF_TYPE_CONCRETE,
        //     SURF_TYPE_DIRT,
        //     SURF_TYPE_FLESH,
        //     SURF_TYPE_FOLIAGE,
        //     SURF_TYPE_GLASS,
        //     SURF_TYPE_GRASS,
        //     SURF_TYPE_GRAVEL,
        //     SURF_TYPE_ICE,
        //     SURF_TYPE_METAL,
        //     SURF_TYPE_MUD,
        //     SURF_TYPE_PAPER,
        //     SURF_TYPE_PLASTER,
        //     SURF_TYPE_ROCK,
        //     SURF_TYPE_SAND,
        //     SURF_TYPE_SNOW,
        //     SURF_TYPE_WATER,
        //     SURF_TYPE_WOOD,
        //     SURF_TYPE_ASPHALT,
        //     SURF_TYPE_CERAMIC,
        //     SURF_TYPE_PLASTIC,
        //     SURF_TYPE_RUBBER,
        //     SURF_TYPE_CUSHION,
        //     SURF_TYPE_FRUIT,
        //     SURF_TYPE_PAINTEDMETAL,
        //     SURF_TYPE_PLAYER,
        //     SURF_TYPE_TALLGRASS,
        //     SURF_TYPE_RIOTSHIELD,
        //     SURF_TYPE_OPAQUEGLASS,
        //     SURF_TYPE_CLIPMISSILE,
        //     SURF_TYPE_AI_NOSIGHT,
        //     SURF_TYPE_CLIPSHOT,
        //     SURF_TYPE_PLAYERCLIP,
        //     SURF_TYPE_MONSTERCLIP,
        //     SURF_TYPE_VEHICLECLIP,
        //     SURF_TYPE_ITEMCLIP,
        //     SURF_TYPE_NODROP,
        //     SURF_TYPE_NONSOLID,
        //     SURF_TYPE_DETAIL,
        //     SURF_TYPE_STRUCTURAL,
        //     SURF_TYPE_PORTAL,
        //     SURF_TYPE_CANSHOOTCLIP,
        //     SURF_TYPE_ORIGIN,
        //     SURF_TYPE_SKY,
        //     SURF_TYPE_NOCASTSHADOW,
        //     SURF_TYPE_ONLYCASTSHADOW,
        //     SURF_TYPE_PHYSICSGEOM,
        //     SURF_TYPE_LIGHTPORTAL,
        //     SURF_TYPE_CAULK,
        //     SURF_TYPE_AREALIGHT,
        //     SURF_TYPE_SLICK,
        //     SURF_TYPE_NOIMPACT,
        //     SURF_TYPE_NOMARKS,
        //     SURF_TYPE_NOPENETRATE,
        //     SURF_TYPE_LADDER,
        //     SURF_TYPE_NODAMAGE,
        //     SURF_TYPE_MANTLEON,
        //     SURF_TYPE_MANTLEOVER,
        //     SURF_TYPE_MOUNT,
        //     SURF_TYPE_NOSTEPS,
        //     SURF_TYPE_NODRAW,
        //     SURF_TYPE_NORECEIVEDYNAMICSHADOW,
        //     SURF_TYPE_NODLIGHT,
        //     SURF_TYPE_COUNT
        // };
        //
        // struct s_SurfaceTypeFlags
        //{
        //     unsigned int surfaceFlags;
        //     unsigned int contentFlags;
        // };
        //
        // s_SurfaceTypeFlags surfaceTypeToFlagMap[SURF_TYPE_COUNT] = {
        //     {0x100000,   0         }, // bark
        //     {0x200000,   0         }, // brick
        //     {0x300000,   0         }, // carpet
        //     {0x400000,   0         }, // cloth
        //     {0x500000,   0         }, // concrete
        //     {0x600000,   0         }, // dirt
        //     {0x700000,   0         }, // flesh
        //     {0x800000,   2         }, // foliage
        //     {0x900000,   0x10      }, // glass
        //     {0x0A00000,  0         }, // grass
        //     {0x0B00000,  0         }, // gravel
        //     {0x0C00000,  0         }, // ice
        //     {0x0D00000,  0         }, // metal
        //     {0x0E00000,  0         }, // mud
        //     {0x0F00000,  0         }, // paper
        //     {0x1000000,  0         }, // plaster
        //     {0x1100000,  0         }, // rock
        //     {0x1200000,  0         }, // sand
        //     {0x1300000,  0         }, // snow
        //     {0x1400000,  0x20      }, // water
        //     {0x1500000,  0         }, // wood
        //     {0x1600000,  0         }, // asphalt
        //     {0x1700000,  0         }, // ceramic
        //     {0x1800000,  0         }, // plastic
        //     {0x1900000,  0         }, // rubber
        //     {0x1A00000,  0         }, // cushion
        //     {0x1B00000,  0         }, // fruit
        //     {0x1C00000,  0         }, // paintedmetal
        //     {0x1D00000,  0         }, // player
        //     {0x1E00000,  0         }, // tallgrass
        //     {0x1F00000,  0         }, // riotshield
        //     {0x900000,   0         }, // opaqueglass
        //     {0,          0x80      }, // clipmissile
        //     {0,          0x1000    }, // ai_nosight
        //     {0,          0x2000    }, // clipshot
        //     {0,          0x10000   }, // playerclip
        //     {0,          0x20000   }, // monsterclip
        //     {0,          0x200     }, // vehicleclip
        //     {0,          0x400     }, // itemclip
        //     {0,          0x80000000}, // noDrop
        //     {0x4000,     0         }, // nonSolid
        //     {0,          0x8000000 }, // detail
        //     {0,          0x10000000}, // structural
        //     {0x80000000, 0         }, // portal
        //     {0,          0x40      }, // canShootClip
        //     {0,          0         }, // origin
        //     {4,          0x800     }, // sky
        //     {0x40000,    0         }, // noCastShadow
        //     {0x80000,    0         }, // onlyCastShadow
        //     {0,          0         }, // physicsGeom
        //     {0,          0         }, // lightPortal
        //     {0x1000,     0         }, // caulk
        //     {0x8000,     0         }, // areaLight
        //     {2,          0         }, // slick
        //     {0x10,       0         }, // noImpact
        //     {0x20,       0         }, // noMarks
        //     {0x100,      0         }, // noPenetrate
        //     {8,          0         }, // ladder
        //     {1,          0         }, // noDamage
        //     {0x4000000,  0x1000000 }, // mantleOn
        //     {0x8000000,  0x1000000 }, // mantleOver
        //     {0x10000000, 0x1000000 }, // mount
        //     {0x2000,     0         }, // noSteps
        //     {0x80,       0         }, // noDraw
        //     {0x800,      0         }, // noReceiveDynamicShadow
        //     {0x20000,    0         }  // noDlight
        // };
        //
        //// TODO: template/define to get the surface type from the surface flags
    } // namespace BSPFlags
} // namespace BSP
