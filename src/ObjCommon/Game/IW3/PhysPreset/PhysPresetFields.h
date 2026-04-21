#pragma once

#include "Game/IW3/IW3.h"

namespace IW3
{
    inline cspField_t phys_preset_fields[]{
        {"sndAliasPrefix",        offsetof(PhysPresetInfo, sndAliasPrefix),        CSPFT_STRING  },
        {"mass",                  offsetof(PhysPresetInfo, mass),                  CSPFT_FLOAT   },
        {"friction",              offsetof(PhysPresetInfo, friction),              CSPFT_FLOAT   },
        {"isFrictionInfinity",    offsetof(PhysPresetInfo, isFrictionInfinity),    CSPFT_QBOOLEAN},
        {"bounce",                offsetof(PhysPresetInfo, bounce),                CSPFT_FLOAT   },
        {"bulletForceScale",      offsetof(PhysPresetInfo, bulletForceScale),      CSPFT_FLOAT   },
        {"explosiveForceScale",   offsetof(PhysPresetInfo, explosiveForceScale),   CSPFT_FLOAT   },
        {"piecesSpreadFraction",  offsetof(PhysPresetInfo, piecesSpreadFraction),  CSPFT_FLOAT   },
        {"piecesUpwardVelocity",  offsetof(PhysPresetInfo, piecesUpwardVelocity),  CSPFT_FLOAT   },
        {"tempDefaultToCylinder", offsetof(PhysPresetInfo, tempDefaultToCylinder), CSPFT_FLOAT   },
    };
}
