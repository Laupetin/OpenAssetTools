#pragma once
#include <string>
#include <ostream>

#include "Zone/Zone.h"

class ZoneWriting
{
public:
    static bool WriteZone(std::ostream& stream, Zone* zone);
};