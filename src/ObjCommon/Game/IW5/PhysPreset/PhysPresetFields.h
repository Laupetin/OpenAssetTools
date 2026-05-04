#pragma once

#include "Game/IW5/IW5.h"

namespace IW5
{
    inline cspField_t phys_preset_fields[]{
        {"mass",                  offsetof(PhysPresetInfo, mass),                  CSPFT_FLOAT   },
        {"bounce",                offsetof(PhysPresetInfo, bounce),                CSPFT_FLOAT   },
        {"friction",              offsetof(PhysPresetInfo, friction),              CSPFT_FLOAT   },
        {"isFrictionInfinity",    offsetof(PhysPresetInfo, isFrictionInfinity),    CSPFT_QBOOLEAN},
        {"bulletForceScale",      offsetof(PhysPresetInfo, bulletForceScale),      CSPFT_FLOAT   },
        {"explosiveForceScale",   offsetof(PhysPresetInfo, explosiveForceScale),   CSPFT_FLOAT   },
        {"sndAliasPrefix",        offsetof(PhysPresetInfo, sndAliasPrefix),        CSPFT_STRING  },
        {"piecesSpreadFraction",  offsetof(PhysPresetInfo, piecesSpreadFraction),  CSPFT_FLOAT   },
        {"piecesUpwardVelocity",  offsetof(PhysPresetInfo, piecesUpwardVelocity),  CSPFT_FLOAT   },
        {"minMomentum",           offsetof(PhysPresetInfo, minMomentum),           CSPFT_FLOAT   },
        {"maxMomentum",           offsetof(PhysPresetInfo, maxMomentum),           CSPFT_FLOAT   },
        {"minPitch",              offsetof(PhysPresetInfo, minPitch),              CSPFT_FLOAT   },
        {"maxPitch",              offsetof(PhysPresetInfo, maxPitch),              CSPFT_FLOAT   },
        {"volumeType",            offsetof(PhysPresetInfo, volumeType),            PPFT_SCALING  },
        {"pitchType",             offsetof(PhysPresetInfo, pitchType),             PPFT_SCALING  },
        {"tempDefaultToCylinder", offsetof(PhysPresetInfo, tempDefaultToCylinder), CSPFT_QBOOLEAN},
        {"perSurfaceSndAlias",    offsetof(PhysPresetInfo, perSurfaceSndAlias),    CSPFT_QBOOLEAN},
    };
}
