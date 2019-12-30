#pragma once

#include "Zone/Zone.h"
#include "SearchPath/ISearchPath.h"

class ObjLoading
{
public:
    static void LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone);
    static void UnloadContainersOfZone(Zone* zone);

    static void LoadObjDataForZone(ISearchPath* searchPath, Zone* zone);
};