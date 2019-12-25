#pragma once

#include "Zone/Zone.h"
#include "Pool/AssetPool.h"

template<class T>
class IAssetDumper
{
public:
    virtual ~IAssetDumper() = default;

    virtual void DumpPool(Zone* zone, AssetPool<T>* pool, const std::string& basePath) = 0;
};