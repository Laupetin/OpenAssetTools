#pragma once

#include "XModel/XModelCommon.h"

#include <memory>

class XModelFileLoader
{
public:
    XModelFileLoader() = default;
    virtual ~XModelFileLoader() = default;
    XModelFileLoader(const XModelFileLoader& other) = default;
    XModelFileLoader(XModelFileLoader&& other) noexcept = default;
    XModelFileLoader& operator=(const XModelFileLoader& other) = default;
    XModelFileLoader& operator=(XModelFileLoader&& other) noexcept = default;

    virtual std::unique_ptr<XModelCommon> Load() = 0;
};
