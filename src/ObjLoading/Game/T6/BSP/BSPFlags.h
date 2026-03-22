#pragma once

namespace BSPFlags
{
    enum SurfaceType
    {
        SURF_TYPE_BARK,
        SURF_TYPE_BRICK,
        SURF_TYPE_CARPET,
        SURF_TYPE_CLOTH,
        SURF_TYPE_CONCRETE,
        SURF_TYPE_DIRT,
        SURF_TYPE_FLESH,
        SURF_TYPE_FOLIAGE,
        SURF_TYPE_GLASS,
        SURF_TYPE_GRASS,
        SURF_TYPE_GRAVEL,
        SURF_TYPE_ICE,
        SURF_TYPE_METAL,
        SURF_TYPE_MUD,
        SURF_TYPE_PAPER,
        SURF_TYPE_PLASTER,
        SURF_TYPE_ROCK,
        SURF_TYPE_SAND,
        SURF_TYPE_SNOW,
        SURF_TYPE_WATER,
        SURF_TYPE_WOOD,
        SURF_TYPE_ASPHALT,
        SURF_TYPE_CERAMIC,
        SURF_TYPE_PLASTIC,
        SURF_TYPE_RUBBER,
        SURF_TYPE_CUSHION,
        SURF_TYPE_FRUIT,
        SURF_TYPE_PAINTEDMETAL,
        SURF_TYPE_PLAYER,
        SURF_TYPE_TALLGRASS,
        SURF_TYPE_RIOTSHIELD,
        SURF_TYPE_OPAQUEGLASS,
        SURF_TYPE_CLIPMISSILE,
        SURF_TYPE_AI_NOSIGHT,
        SURF_TYPE_CLIPSHOT,
        SURF_TYPE_PLAYERCLIP,
        SURF_TYPE_MONSTERCLIP,
        SURF_TYPE_VEHICLECLIP,
        SURF_TYPE_ITEMCLIP,
        SURF_TYPE_NODROP,
        SURF_TYPE_NONSOLID,
        SURF_TYPE_DETAIL,
        SURF_TYPE_STRUCTURAL,
        SURF_TYPE_PORTAL,
        SURF_TYPE_CANSHOOTCLIP,
        SURF_TYPE_ORIGIN,
        SURF_TYPE_SKY,
        SURF_TYPE_NOCASTSHADOW,
        SURF_TYPE_CASTSUNSHADOW,
        SURF_TYPE_PHYSICSGEOM,
        SURF_TYPE_LIGHTPORTAL,
        SURF_TYPE_CAULK,
        SURF_TYPE_AREALIGHT,
        SURF_TYPE_SLICK,
        SURF_TYPE_NOIMPACT,
        SURF_TYPE_NOMARKS,
        SURF_TYPE_NOPENETRATE,
        SURF_TYPE_LADDER,
        SURF_TYPE_NODAMAGE,
        SURF_TYPE_MANTLEON,
        SURF_TYPE_MANTLEOVER,
        SURF_TYPE_MOUNT,
        SURF_TYPE_NOSTEPS,
        SURF_TYPE_NODRAW,
        SURF_TYPE_NORECEIVEDYNAMICSHADOW,
        SURF_TYPE_NODLIGHT,
        SURF_TYPE_NONE, // Custom value
        SURF_TYPE_COUNT
    };

    struct s_SurfaceTypeFlags
    {
        unsigned int surfaceFlags;
        unsigned int contentFlags;
    };

    constexpr s_SurfaceTypeFlags surfaceTypeToFlagMap[SURF_TYPE_COUNT] = {
        {0x100000,   0         }, // bark
        {0x200000,   0         }, // brick
        {0x300000,   0         }, // carpet
        {0x400000,   0         }, // cloth
        {0x500000,   0         }, // concrete
        {0x600000,   0         }, // dirt
        {0x700000,   0         }, // flesh
        {0x800000,   2         }, // foliage
        {0x900000,   0x10      }, // glass
        {0x0A00000,  0         }, // grass
        {0x0B00000,  0         }, // gravel
        {0x0C00000,  0         }, // ice
        {0x0D00000,  0         }, // metal
        {0x0E00000,  0         }, // mud
        {0x0F00000,  0         }, // paper
        {0x1000000,  0         }, // plaster
        {0x1100000,  0         }, // rock
        {0x1200000,  0         }, // sand
        {0x1300000,  0         }, // snow
        {0x1400000,  0x20      }, // water
        {0x1500000,  0         }, // wood
        {0x1600000,  0         }, // asphalt
        {0x1700000,  0         }, // ceramic
        {0x1800000,  0         }, // plastic
        {0x1900000,  0         }, // rubber
        {0x1A00000,  0         }, // cushion
        {0x1B00000,  0         }, // fruit
        {0x1C00000,  0         }, // paintedmetal
        {0x1D00000,  0         }, // player
        {0x1E00000,  0         }, // tallgrass
        {0x1F00000,  0         }, // riotshield
        {0x900000,   0         }, // opaqueglass
        {0,          0x80      }, // clipmissile
        {0,          0x1000    }, // ai_nosight
        {0,          0x2000    }, // clipshot
        {0,          0x10000   }, // playerclip
        {0,          0x20000   }, // monsterclip
        {0,          0x200     }, // vehicleclip
        {0,          0x400     }, // itemclip
        {0,          0x80000000}, // noDrop
        {0x4000,     0         }, // nonSolid
        {0,          0x8000000 }, // detail
        {0,          0x10000000}, // structural
        {0x80000000, 0         }, // portal
        {0,          0x40      }, // canShootClip
        {0,          0         }, // origin
        {4,          0x800     }, // sky
        {0x40000,    0         }, // noCastShadow
        {0x80000,    0         }, // castSunShadow
        {0,          0         }, // physicsGeom
        {0,          0         }, // lightPortal
        {0x1000,     0         }, // caulk
        {0x8000,     0         }, // areaLight
        {2,          0         }, // slick
        {0x10,       0         }, // noImpact
        {0x20,       0         }, // noMarks
        {0x100,      0         }, // noPenetrate
        {8,          0         }, // ladder
        {1,          0         }, // noDamage
        {0x4000000,  0x1000000 }, // mantleOn
        {0x8000000,  0x1000000 }, // mantleOver
        {0x10000000, 0x1000000 }, // mount
        {0x2000,     0         }, // noSteps
        {0x80,       0         }, // noDraw
        {0x800,      0         }, // noReceiveDynamicShadow
        {0x20000,    0         }, // noDlight
        {0,          0         }  // none
    };

    constexpr SurfaceType materialTypes[] = {
        SURF_TYPE_NONE,         SURF_TYPE_BARK,    SURF_TYPE_BRICK,     SURF_TYPE_CARPET,     SURF_TYPE_CLOTH,       SURF_TYPE_CONCRETE, SURF_TYPE_DIRT,
        SURF_TYPE_FLESH,        SURF_TYPE_FOLIAGE, SURF_TYPE_GLASS,     SURF_TYPE_GRASS,      SURF_TYPE_GRAVEL,      SURF_TYPE_ICE,      SURF_TYPE_METAL,
        SURF_TYPE_MUD,          SURF_TYPE_PAPER,   SURF_TYPE_PLASTER,   SURF_TYPE_ROCK,       SURF_TYPE_SAND,        SURF_TYPE_SNOW,     SURF_TYPE_WATER,
        SURF_TYPE_WOOD,         SURF_TYPE_ASPHALT, SURF_TYPE_CERAMIC,   SURF_TYPE_PLASTIC,    SURF_TYPE_RUBBER,      SURF_TYPE_CUSHION,  SURF_TYPE_FRUIT,
        SURF_TYPE_PAINTEDMETAL, SURF_TYPE_PLAYER,  SURF_TYPE_TALLGRASS, SURF_TYPE_RIOTSHIELD, SURF_TYPE_OPAQUEGLASS,
    };

    constexpr SurfaceType materialFlags[] = {
        SURF_TYPE_NONE,
        SURF_TYPE_CLIPMISSILE,
        SURF_TYPE_AI_NOSIGHT,
        SURF_TYPE_CLIPSHOT,
        SURF_TYPE_PLAYERCLIP,
        SURF_TYPE_MONSTERCLIP,
        SURF_TYPE_VEHICLECLIP,
        SURF_TYPE_ITEMCLIP,
        SURF_TYPE_NODROP,
        SURF_TYPE_NONSOLID,
        SURF_TYPE_DETAIL,
        SURF_TYPE_STRUCTURAL,
        SURF_TYPE_PORTAL,
        SURF_TYPE_CANSHOOTCLIP,
        SURF_TYPE_SKY,
        SURF_TYPE_NOCASTSHADOW,
        SURF_TYPE_CASTSUNSHADOW,
        SURF_TYPE_CAULK,
        SURF_TYPE_AREALIGHT,
        SURF_TYPE_SLICK,
        SURF_TYPE_NOIMPACT,
        SURF_TYPE_NOMARKS,
        SURF_TYPE_NOPENETRATE,
        SURF_TYPE_LADDER,
        SURF_TYPE_NODAMAGE,
        SURF_TYPE_MANTLEON,
        SURF_TYPE_MANTLEOVER,
        SURF_TYPE_MOUNT,
        SURF_TYPE_NOSTEPS,
        SURF_TYPE_NODRAW,
        SURF_TYPE_NORECEIVEDYNAMICSHADOW,
        SURF_TYPE_NODLIGHT,
    };

    constexpr const char* surfaceTypeToNameMap[SURF_TYPE_COUNT] = {"bark",        "brick",       "carpet",       "cloth",      "concrete",
                                                                   "dirt",        "flesh",       "foliage",      "glass",      "grass",
                                                                   "gravel",      "ice",         "metal",        "mud",        "paper",
                                                                   "plaster",     "rock",        "sand",         "snow",       "water",
                                                                   "wood",        "asphalt",     "ceramic",      "plastic",    "rubber",
                                                                   "cushion",     "fruit",       "paintedmetal", "player",     "tallgrass",
                                                                   "riotshield",  "opaqueglass", "clipmissile",  "ai_nosight", "clipshot",
                                                                   "playerclip",  "monsterclip", "vehicleclip",  "itemclip",   "nodrop",
                                                                   "nonsolid",    "detail",      "structural",   "portal",     "canshootclip",
                                                                   "origin",      "sky",         "nocastshadow", "castshadow", "physicsgeom",
                                                                   "lightportal", "caulk",       "arealight",    "slick",      "noimpact",
                                                                   "nomarks",     "nopenetrate", "ladder",       "nodamage",   "mantleon",
                                                                   "mantleover",  "mount",       "nosteps",      "nodraw",     "noreceivedynamicshadow",
                                                                   "nodlight",    "none"};
} // namespace BSPFlags
