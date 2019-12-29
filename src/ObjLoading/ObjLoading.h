#pragma once

#include "Zone/Zone.h"

class ObjLoading
{
public:
    static void LoadReferencedContainersForZone(Zone* zone);
    static void LoadObjDataForZone(Zone* zone);
    static void UnloadContainersOfZone(Zone* zone);
};