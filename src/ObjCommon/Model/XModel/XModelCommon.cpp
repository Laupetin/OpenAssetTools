#include "XModelCommon.h"

void XModelMaterial::ApplyDefaults()
{
    // Phong = Color, Bump, Spec, CosinePower
    // Blinn = Color, Bump, Spec, Eccentricity
    // Lambert = Color, Bump
    materialTypeName = "Phong";
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
    color[3] = 1;
    transparency[0] = 0;
    transparency[1] = 0;
    transparency[2] = 0;
    transparency[3] = 1;
    ambientColor[0] = 0;
    ambientColor[1] = 0;
    ambientColor[2] = 0;
    ambientColor[3] = 1;
    incandescence[0] = 0;
    incandescence[1] = 0;
    incandescence[2] = 0;
    incandescence[3] = 1;
    coeffs[0] = 0.8f;
    coeffs[1] = 0;
    glow.x = 0;
    glow.y = 0;
    refractive.x = 6;
    refractive.y = 1;
    specularColor[0] = -1;
    specularColor[1] = -1;
    specularColor[2] = -1;
    specularColor[3] = 1;
    reflectiveColor[0] = -1;
    reflectiveColor[1] = -1;
    reflectiveColor[2] = -1;
    reflectiveColor[3] = 1;
    reflective.x = -1;
    reflective.y = -1;
    blinn[0] = -1;
    blinn[1] = -1;
    phong = -1;
}
