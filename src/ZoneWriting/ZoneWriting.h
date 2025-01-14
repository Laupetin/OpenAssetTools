#pragma once

#include "Zone/Zone.h"

#include <ostream>

class ZoneWriting
{
public:
    static bool WriteZone(std::ostream& stream, const Zone& zone);
};
