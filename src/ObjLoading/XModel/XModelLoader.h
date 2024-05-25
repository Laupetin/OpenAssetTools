#pragma once

#include "XModel/XModelCommon.h"

#include <memory>

class XModelLoader
{
public:
    XModelLoader() = default;
    virtual ~XModelLoader() = default;
    XModelLoader(const XModelLoader& other) = default;
    XModelLoader(XModelLoader&& other) noexcept = default;
    XModelLoader& operator=(const XModelLoader& other) = default;
    XModelLoader& operator=(XModelLoader&& other) noexcept = default;

    virtual std::unique_ptr<XModelCommon> Load() = 0;
};
