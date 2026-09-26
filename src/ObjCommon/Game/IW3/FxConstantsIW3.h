#pragma once

#include "Fx/FxFlagDef.h"
#include "Game/IW3/IW3.h"

#include <array>

namespace IW3
{
    using ::fx::FxFlagDef;
    using ::fx::FxFlagType;

    enum class FxImpactTableIndex : unsigned
    {
        BULLET_SMALL_NORMAL,
        BULLET_SMALL_EXIT,
        BULLET_LARGE_NORMAL,
        BULLET_LARGE_EXIT,
        SHOTGUN_NORMAL,
        SHOTGUN_EXIT,
        BULLET_AP_NORMAL,
        BULLET_AP_EXIT,
        GRENADE_BOUNCE,
        GRENADE_EXPLODE,
        ROCKET_EXPLODE,
        PROJECTILE_DUD,
        COUNT,
    };

    struct FxImpactTypeDef
    {
        const char* name;
        FxImpactTableIndex tableIndex;
    };

    inline constexpr std::array FX_IMPACT_TYPE_DEFS{
        FxImpactTypeDef{"bullet_small_normal", FxImpactTableIndex::BULLET_SMALL_NORMAL},
        FxImpactTypeDef{"bullet_small_exit",   FxImpactTableIndex::BULLET_SMALL_EXIT  },
        FxImpactTypeDef{"bullet_large_normal", FxImpactTableIndex::BULLET_LARGE_NORMAL},
        FxImpactTypeDef{"bullet_large_exit",   FxImpactTableIndex::BULLET_LARGE_EXIT  },
        FxImpactTypeDef{"bullet_ap_normal",    FxImpactTableIndex::BULLET_AP_NORMAL   },
        FxImpactTypeDef{"bullet_ap_exit",      FxImpactTableIndex::BULLET_AP_EXIT     },
        FxImpactTypeDef{"shotgun_normal",      FxImpactTableIndex::SHOTGUN_NORMAL     },
        FxImpactTypeDef{"shotgun_exit",        FxImpactTableIndex::SHOTGUN_EXIT       },
        FxImpactTypeDef{"grenade_bounce",      FxImpactTableIndex::GRENADE_BOUNCE     },
        FxImpactTypeDef{"grenade_explode",     FxImpactTableIndex::GRENADE_EXPLODE    },
        FxImpactTypeDef{"rocket_explode",      FxImpactTableIndex::ROCKET_EXPLODE     },
        FxImpactTypeDef{"projectile_dud",      FxImpactTableIndex::PROJECTILE_DUD     },
    };
    static_assert(std::size(FX_IMPACT_TYPE_DEFS) == static_cast<unsigned>(FxImpactTableIndex::COUNT));

    inline constexpr std::array FX_FLAG_DEFS{
        FxFlagDef{"looping",             FxFlagType::EDITOR, FX_ED_FLAG_LOOPING,                   FX_ED_FLAG_LOOPING                  },
        FxFlagDef{"useRandColor",        FxFlagType::EDITOR, FX_ED_FLAG_USE_RANDOM_COLOR,          FX_ED_FLAG_USE_RANDOM_COLOR         },
        FxFlagDef{"useRandAlpha",        FxFlagType::EDITOR, FX_ED_FLAG_USE_RANDOM_ALPHA,          FX_ED_FLAG_USE_RANDOM_ALPHA         },
        FxFlagDef{"useRandSize0",        FxFlagType::EDITOR, FX_ED_FLAG_USE_RANDOM_SIZE_0,         FX_ED_FLAG_USE_RANDOM_SIZE_0        },
        FxFlagDef{"useRandSize1",        FxFlagType::EDITOR, FX_ED_FLAG_USE_RANDOM_SIZE_1,         FX_ED_FLAG_USE_RANDOM_SIZE_1        },
        FxFlagDef{"useRandScale",        FxFlagType::EDITOR, FX_ED_FLAG_USE_RANDOM_SCALE,          FX_ED_FLAG_USE_RANDOM_SCALE         },
        FxFlagDef{"useRandRotDelta",     FxFlagType::EDITOR, FX_ED_FLAG_USE_RANDOM_ROTATION_DELTA, FX_ED_FLAG_USE_RANDOM_ROTATION_DELTA},
        FxFlagDef{"modColorByAlpha",     FxFlagType::EDITOR, FX_ED_FLAG_MODULATE_COLOR_BY_ALPHA,   FX_ED_FLAG_MODULATE_COLOR_BY_ALPHA  },
        FxFlagDef{"useRandVel0",         FxFlagType::EDITOR, FX_ED_FLAG_USE_RANDOM_VELOCITY_0,     FX_ED_FLAG_USE_RANDOM_VELOCITY_0    },
        FxFlagDef{"useRandVel1",         FxFlagType::EDITOR, FX_ED_FLAG_USE_RANDOM_VELOCITY_1,     FX_ED_FLAG_USE_RANDOM_VELOCITY_1    },
        FxFlagDef{"useBackCompatVel",    FxFlagType::EDITOR, FX_ED_FLAG_BACKCOMPAT_VELOCITY,       FX_ED_FLAG_BACKCOMPAT_VELOCITY      },
        FxFlagDef{"absVel0",             FxFlagType::EDITOR, FX_ED_FLAG_ABSOLUTE_VELOCITY_0,       FX_ED_FLAG_ABSOLUTE_VELOCITY_0      },
        FxFlagDef{"absVel1",             FxFlagType::EDITOR, FX_ED_FLAG_ABSOLUTE_VELOCITY_1,       FX_ED_FLAG_ABSOLUTE_VELOCITY_1      },
        FxFlagDef{"playOnTouch",         FxFlagType::EDITOR, FX_ED_FLAG_PLAY_ON_TOUCH,             FX_ED_FLAG_PLAY_ON_TOUCH            },
        FxFlagDef{"playOnDeath",         FxFlagType::EDITOR, FX_ED_FLAG_PLAY_ON_DEATH,             FX_ED_FLAG_PLAY_ON_DEATH            },
        FxFlagDef{"playOnRun",           FxFlagType::EDITOR, FX_ED_FLAG_PLAY_ON_RUN,               FX_ED_FLAG_PLAY_ON_RUN              },
        FxFlagDef{"boundingSphere",      FxFlagType::EDITOR, FX_ED_FLAG_BOUNDING_SPHERE,           FX_ED_FLAG_BOUNDING_SPHERE          },
        FxFlagDef{"useItemClip",         FxFlagType::EDITOR, FX_ED_FLAG_USE_ITEM_CLIP,             FX_ED_FLAG_USE_ITEM_CLIP            },
        FxFlagDef{"disabled",            FxFlagType::EDITOR, FX_ED_FLAG_DISABLED,                  FX_ED_FLAG_DISABLED                 },
        FxFlagDef{"spawnRelative",       FxFlagType::NATIVE, FX_ELEM_SPAWN_RELATIVE_TO_EFFECT,     FX_ELEM_SPAWN_RELATIVE_TO_EFFECT    },
        FxFlagDef{"spawnFrustumCull",    FxFlagType::NATIVE, FX_ELEM_SPAWN_FRUSTUM_CULL,           FX_ELEM_SPAWN_FRUSTUM_CULL          },
        FxFlagDef{"runnerUsesRandRot",   FxFlagType::NATIVE, FX_ELEM_RUNNER_USES_RAND_ROT,         FX_ELEM_RUNNER_USES_RAND_ROT        },
        FxFlagDef{"spawnOffsetNone",     FxFlagType::NATIVE, FX_ELEM_SPAWN_OFFSET_MASK,            FX_ELEM_SPAWN_OFFSET_NONE           },
        FxFlagDef{"spawnOffsetSphere",   FxFlagType::NATIVE, FX_ELEM_SPAWN_OFFSET_MASK,            FX_ELEM_SPAWN_OFFSET_SPHERE         },
        FxFlagDef{"spawnOffsetCylinder", FxFlagType::NATIVE, FX_ELEM_SPAWN_OFFSET_MASK,            FX_ELEM_SPAWN_OFFSET_CYLINDER       },
        FxFlagDef{"runRelToWorld",       FxFlagType::NATIVE, FX_ELEM_RUN_MASK,                     FX_ELEM_RUN_RELATIVE_TO_WORLD       },
        FxFlagDef{"runRelToSpawn",       FxFlagType::NATIVE, FX_ELEM_RUN_MASK,                     FX_ELEM_RUN_RELATIVE_TO_SPAWN       },
        FxFlagDef{"runRelToEffect",      FxFlagType::NATIVE, FX_ELEM_RUN_MASK,                     FX_ELEM_RUN_RELATIVE_TO_EFFECT      },
        FxFlagDef{"runRelToOffset",      FxFlagType::NATIVE, FX_ELEM_RUN_MASK,                     FX_ELEM_RUN_RELATIVE_TO_OFFSET      },
        FxFlagDef{"useCollision",        FxFlagType::NATIVE, FX_ELEM_USE_COLLISION,                FX_ELEM_USE_COLLISION               },
        FxFlagDef{"dieOnTouch",          FxFlagType::NATIVE, FX_ELEM_DIE_ON_TOUCH,                 FX_ELEM_DIE_ON_TOUCH                },
        FxFlagDef{"drawPastFog",         FxFlagType::NATIVE, FX_ELEM_DRAW_PAST_FOG,                FX_ELEM_DRAW_PAST_FOG               },
        FxFlagDef{"drawWithViewModel",   FxFlagType::NATIVE, FX_ELEM_DRAW_WITH_VIEWMODEL,          FX_ELEM_DRAW_WITH_VIEWMODEL         },
        FxFlagDef{"blocksSight",         FxFlagType::NATIVE, FX_ELEM_BLOCK_SIGHT,                  FX_ELEM_BLOCK_SIGHT                 },
        FxFlagDef{"modelUsesPhysics",    FxFlagType::NATIVE, FX_ELEM_USE_MODEL_PHYSICS,            FX_ELEM_USE_MODEL_PHYSICS           },
        FxFlagDef{"nonUniformScale",     FxFlagType::NATIVE, FX_ELEM_NONUNIFORM_SCALE,             FX_ELEM_NONUNIFORM_SCALE            },
        FxFlagDef{"startFixed",          FxFlagType::ATLAS,  FX_ATLAS_START_MASK,                  FX_ATLAS_START_FIXED                },
        FxFlagDef{"startRandom",         FxFlagType::ATLAS,  FX_ATLAS_START_MASK,                  FX_ATLAS_START_RANDOM               },
        FxFlagDef{"startIndexed",        FxFlagType::ATLAS,  FX_ATLAS_START_MASK,                  FX_ATLAS_START_INDEXED              },
        FxFlagDef{"playOverLife",        FxFlagType::ATLAS,  FX_ATLAS_PLAY_OVER_LIFE,              FX_ATLAS_PLAY_OVER_LIFE             },
        FxFlagDef{"loopOnlyNTimes",      FxFlagType::ATLAS,  FX_ATLAS_LOOP_ONLY_N_TIMES,           FX_ATLAS_LOOP_ONLY_N_TIMES          },
    };
} // namespace IW3
