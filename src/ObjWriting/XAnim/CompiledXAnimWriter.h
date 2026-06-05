#pragma once

#include "XAnim/BinaryXAnimCommon.h"
#include "XAnim/XAnimCommon.h"

#include <ostream>

namespace xanim
{
    void WriteCompiledXAnim(std::ostream& stream, const CommonXAnimParts& parts, CompiledXAnimVersion version);
}
