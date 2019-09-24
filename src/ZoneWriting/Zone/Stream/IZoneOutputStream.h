#pragma once

class IZoneOutputStream
{
public:
    virtual void Align(int alignTo) = 0;
};