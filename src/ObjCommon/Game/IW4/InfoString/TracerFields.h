#pragma once
#include "Game/IW4/IW4.h"

namespace IW4
{
    inline cspField_t tracer_fields[]{
        {"material",     offsetof(TracerDef, material),     CSPFT_MATERIAL},
        {"drawInterval", offsetof(TracerDef, drawInterval), CSPFT_INT     },
        {"speed",        offsetof(TracerDef, speed),        CSPFT_FLOAT   },
        {"beamLength",   offsetof(TracerDef, beamLength),   CSPFT_FLOAT   },
        {"beamWidth",    offsetof(TracerDef, beamWidth),    CSPFT_FLOAT   },
        {"screwRadius",  offsetof(TracerDef, screwRadius),  CSPFT_FLOAT   },
        {"screwDist",    offsetof(TracerDef, screwDist),    CSPFT_FLOAT   },
        {"colorR0",      offsetof(TracerDef, colors[0][0]), CSPFT_FLOAT   },
        {"colorG0",      offsetof(TracerDef, colors[0][1]), CSPFT_FLOAT   },
        {"colorB0",      offsetof(TracerDef, colors[0][2]), CSPFT_FLOAT   },
        {"colorA0",      offsetof(TracerDef, colors[0][3]), CSPFT_FLOAT   },
        {"colorR1",      offsetof(TracerDef, colors[1][0]), CSPFT_FLOAT   },
        {"colorG1",      offsetof(TracerDef, colors[1][1]), CSPFT_FLOAT   },
        {"colorB1",      offsetof(TracerDef, colors[1][2]), CSPFT_FLOAT   },
        {"colorA1",      offsetof(TracerDef, colors[1][3]), CSPFT_FLOAT   },
        {"colorR2",      offsetof(TracerDef, colors[2][0]), CSPFT_FLOAT   },
        {"colorG2",      offsetof(TracerDef, colors[2][1]), CSPFT_FLOAT   },
        {"colorB2",      offsetof(TracerDef, colors[2][2]), CSPFT_FLOAT   },
        {"colorA2",      offsetof(TracerDef, colors[2][3]), CSPFT_FLOAT   },
        {"colorR3",      offsetof(TracerDef, colors[3][0]), CSPFT_FLOAT   },
        {"colorG3",      offsetof(TracerDef, colors[3][1]), CSPFT_FLOAT   },
        {"colorB3",      offsetof(TracerDef, colors[3][2]), CSPFT_FLOAT   },
        {"colorA3",      offsetof(TracerDef, colors[3][3]), CSPFT_FLOAT   },
        {"colorR4",      offsetof(TracerDef, colors[4][0]), CSPFT_FLOAT   },
        {"colorG4",      offsetof(TracerDef, colors[4][1]), CSPFT_FLOAT   },
        {"colorB4",      offsetof(TracerDef, colors[4][2]), CSPFT_FLOAT   },
        {"colorA4",      offsetof(TracerDef, colors[4][3]), CSPFT_FLOAT   },
    };
}
