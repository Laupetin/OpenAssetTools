#pragma once

#include "XAnimParts/XAnimPartsCommon.h"

#include <vector>

class XAnimPartsWriter
{
public:
    XAnimPartsWriter() = default;
    virtual ~XAnimPartsWriter() = default;
    XAnimPartsWriter(const XAnimPartsWriter& other) = default;
    XAnimPartsWriter(XAnimPartsWriter&& other) noexcept = default;
    XAnimPartsWriter& operator=(const XAnimPartsWriter& other) = default;
    XAnimPartsWriter& operator=(XAnimPartsWriter&& other) noexcept = default;

    virtual void Write(const XAnimPartsCommon& xmodel) = 0;
};
