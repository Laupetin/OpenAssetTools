#pragma once

#include "SearchPath/ISearchPath.h"
#include "Zone/Zone.h"

class IObjLoader
{
public:
    virtual ~IObjLoader() = default;

    virtual bool SupportsZone(Zone* zone) = 0;

    virtual void LoadReferencedContainersForZone(ISearchPath* searchPath, Zone* zone) = 0;
    virtual void UnloadContainersOfZone(Zone* zone) = 0;

    virtual void LoadObjDataForZone(ISearchPath* searchPath, Zone* zone) = 0;
};