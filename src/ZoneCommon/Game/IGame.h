#pragma once
#include "Zone/Zone.h"
#include <vector>

class Zone;

class IGame
{
public:
    virtual void AddZone(Zone* zone) = 0;
    virtual std::vector<Zone*> GetZones() = 0;
};