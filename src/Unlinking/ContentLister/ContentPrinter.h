#pragma once

#include "Zone/Zone.h"

class ContentPrinter
{
public:
    explicit ContentPrinter(const Zone& zone);

    void PrintContent() const;

private:
    const Zone& m_zone;
};
