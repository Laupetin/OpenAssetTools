#pragma once

#include "XModel/XModelWriter.h"

#include <memory>
#include <ostream>

namespace xmodel_export
{
    std::unique_ptr<XModelWriter> CreateWriterForVersion6(std::ostream& stream, std::string gameName, std::string zoneName);
}
