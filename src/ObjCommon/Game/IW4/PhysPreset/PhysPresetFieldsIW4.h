#pragma once

#include "Game/IW4/IW4.h"

namespace IW4
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
        {"tempDefaultToCylinder", offsetof(PhysPresetInfo, tempDefaultToCylinder), CSPFT_QBOOLEAN},
        {"perSurfaceSndAlias",    offsetof(PhysPresetInfo, perSurfaceSndAlias),    CSPFT_QBOOLEAN},
    };
}
