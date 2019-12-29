#pragma once

#include "Zone/Zone.h"

class IObjLoader
{
public:
    virtual ~IObjLoader() = default;

    virtual bool SupportsZone(Zone* zone) = 0;
    virtual void LoadReferencedContainersForZone(Zone* zone) = 0;
    virtual void LoadObjDataForZone(Zone* zone) = 0;
};