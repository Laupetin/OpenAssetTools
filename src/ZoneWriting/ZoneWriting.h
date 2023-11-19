#pragma once
#include "Zone/Zone.h"

#include <ostream>
#include <string>

class ZoneWriting
{
public:
    static bool WriteZone(std::ostream& stream, Zone* zone);
};
