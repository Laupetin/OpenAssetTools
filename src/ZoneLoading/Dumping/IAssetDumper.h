#pragma once

#include "Pool/AssetPool.h"

template<class T>
class IAssetDumper
{
public:
    virtual ~IAssetDumper() = default;

    virtual void DumpPool(AssetPool<T>* pool, const std::string& basePath) = 0;
};