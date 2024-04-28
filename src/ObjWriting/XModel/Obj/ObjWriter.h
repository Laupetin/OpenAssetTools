#pragma once

#include "XModel/XModelWriter.h"

#include <ostream>

namespace obj
{
    std::unique_ptr<XModelWriter> CreateObjWriter(std::ostream& stream, std::string mtlName, std::string gameName, std::string zoneName);
    std::unique_ptr<XModelWriter> CreateMtlWriter(std::ostream& stream, std::string gameName, std::string zoneName);
} // namespace obj
