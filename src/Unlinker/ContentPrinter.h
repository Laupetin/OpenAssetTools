#pragma once

#include "Zone/Zone.h"

class ContentPrinter
{
    Zone* m_zone;

public:
    explicit ContentPrinter(Zone* zone);

    void PrintContent() const;
};