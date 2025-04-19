#pragma once

#include "XModel/XModelWriter.h"

#include <memory>
#include <ostream>

namespace xmodel_bin
{
    std::unique_ptr<XModelWriter> CreateWriterForVersion7(std::ostream& stream, std::string gameName, std::string zoneName);
}
