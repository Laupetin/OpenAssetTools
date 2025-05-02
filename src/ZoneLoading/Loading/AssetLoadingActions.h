#pragma once

#include "Zone/Zone.h"

class AssetLoadingActions
{
public:
    explicit AssetLoadingActions(Zone& zone);

protected:
    Zone& m_zone;
};
