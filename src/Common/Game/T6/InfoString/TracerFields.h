#pragma once
#include "Game/T6/T6.h"

namespace T6
{
    inline cspField_t tracer_fields[]
    {
        {"type", offsetof(TracerDef, type), TFT_TRACERTYPE},
        {"material", offsetof(TracerDef, material), CSPFT_MATERIAL},
        {"drawInterval", offsetof(TracerDef, drawInterval), CSPFT_INT},
        {"speed", offsetof(TracerDef, speed), CSPFT_FLOAT},
        {"beamLength", offsetof(TracerDef, beamLength), CSPFT_FLOAT},
        {"beamWidth", offsetof(TracerDef, beamWidth), CSPFT_FLOAT},
        {"screwRadius", offsetof(TracerDef, screwRadius), CSPFT_FLOAT},
        {"screwDist", offsetof(TracerDef, screwDist), CSPFT_FLOAT},
        {"fadeTime", offsetof(TracerDef, fadeTime), CSPFT_FLOAT},
        {"fadeScale", offsetof(TracerDef, fadeScale), CSPFT_FLOAT},
        {"texRepeatRate", offsetof(TracerDef, texRepeatRate), CSPFT_FLOAT},
        {"colorR0", offsetof(TracerDef, colors[0].r), CSPFT_FLOAT},
        {"colorG0", offsetof(TracerDef, colors[0].g), CSPFT_FLOAT},
        {"colorB0", offsetof(TracerDef, colors[0].b), CSPFT_FLOAT},
        {"colorA0", offsetof(TracerDef, colors[0].a), CSPFT_FLOAT},
        {"colorR1", offsetof(TracerDef, colors[1].r), CSPFT_FLOAT},
        {"colorG1", offsetof(TracerDef, colors[1].g), CSPFT_FLOAT},
        {"colorB1", offsetof(TracerDef, colors[1].b), CSPFT_FLOAT},
        {"colorA1", offsetof(TracerDef, colors[1].a), CSPFT_FLOAT},
        {"colorR2", offsetof(TracerDef, colors[2].r), CSPFT_FLOAT},
        {"colorG2", offsetof(TracerDef, colors[2].g), CSPFT_FLOAT},
        {"colorB2", offsetof(TracerDef, colors[2].b), CSPFT_FLOAT},
        {"colorA2", offsetof(TracerDef, colors[2].a), CSPFT_FLOAT},
        {"colorR3", offsetof(TracerDef, colors[3].r), CSPFT_FLOAT},
        {"colorG3", offsetof(TracerDef, colors[3].g), CSPFT_FLOAT},
        {"colorB3", offsetof(TracerDef, colors[3].b), CSPFT_FLOAT},
        {"colorA3", offsetof(TracerDef, colors[3].a), CSPFT_FLOAT},
        {"colorR4", offsetof(TracerDef, colors[4].r), CSPFT_FLOAT},
        {"colorG4", offsetof(TracerDef, colors[4].g), CSPFT_FLOAT},
        {"colorB4", offsetof(TracerDef, colors[4].b), CSPFT_FLOAT},
        {"colorA4", offsetof(TracerDef, colors[4].a), CSPFT_FLOAT}
    };
}