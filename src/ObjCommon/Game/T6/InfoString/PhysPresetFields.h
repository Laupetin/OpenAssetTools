#pragma once
#include "Game/T6/T6.h"

namespace T6
{
    inline cspField_t phys_preset_fields[]{
        {"mass",                 offsetof(PhysPresetInfo, mass),                 CSPFT_FLOAT   },
        {"bounce",               offsetof(PhysPresetInfo, bounce),               CSPFT_FLOAT   },
        {"friction",             offsetof(PhysPresetInfo, friction),             CSPFT_FLOAT   },
        {"isFrictionInfinity",   offsetof(PhysPresetInfo, isFrictionInfinity),   CSPFT_QBOOLEAN},
        {"bulletForceScale",     offsetof(PhysPresetInfo, bulletForceScale),     CSPFT_FLOAT   },
        {"explosiveForceScale",  offsetof(PhysPresetInfo, explosiveForceScale),  CSPFT_FLOAT   },
        {"piecesSpreadFraction", offsetof(PhysPresetInfo, piecesSpreadFraction), CSPFT_FLOAT   },
        {"piecesUpwardVelocity", offsetof(PhysPresetInfo, piecesUpwardVelocity), CSPFT_FLOAT   },
        {"canFloat",             offsetof(PhysPresetInfo, canFloat),             CSPFT_INT     },
        {"gravityScale",         offsetof(PhysPresetInfo, gravityScale),         CSPFT_FLOAT   },
        {"massOffsetX",          offsetof(PhysPresetInfo, centerOfMassOffset.x), CSPFT_FLOAT   },
        {"massOffsetY",          offsetof(PhysPresetInfo, centerOfMassOffset.y), CSPFT_FLOAT   },
        {"massOffsetZ",          offsetof(PhysPresetInfo, centerOfMassOffset.z), CSPFT_FLOAT   },
        {"buoyancyMinX",         offsetof(PhysPresetInfo, buoyancyBoxMin.x),     CSPFT_FLOAT   },
        {"buoyancyMinY",         offsetof(PhysPresetInfo, buoyancyBoxMin.y),     CSPFT_FLOAT   },
        {"buoyancyMinZ",         offsetof(PhysPresetInfo, buoyancyBoxMin.z),     CSPFT_FLOAT   },
        {"buoyancyMaxX",         offsetof(PhysPresetInfo, buoyancyBoxMax.x),     CSPFT_FLOAT   },
        {"buoyancyMaxY",         offsetof(PhysPresetInfo, buoyancyBoxMax.y),     CSPFT_FLOAT   },
        {"buoyancyMaxZ",         offsetof(PhysPresetInfo, buoyancyBoxMax.z),     CSPFT_FLOAT   },
    };
}
