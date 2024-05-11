#pragma once

#include "XModel/XModelCommon.h"

#include <vector>

class XModelWriter
{
public:
    XModelWriter() = default;
    virtual ~XModelWriter() = default;
    XModelWriter(const XModelWriter& other) = default;
    XModelWriter(XModelWriter&& other) noexcept = default;
    XModelWriter& operator=(const XModelWriter& other) = default;
    XModelWriter& operator=(XModelWriter&& other) noexcept = default;

    virtual void Write(const XModelCommon& xmodel) = 0;
};
