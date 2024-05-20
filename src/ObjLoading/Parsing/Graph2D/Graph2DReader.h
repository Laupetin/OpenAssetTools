#pragma once

#include "Parsing/GenericGraph2D.h"

#include <istream>
#include <memory>

namespace graph2d
{
    [[nodiscard]] std::unique_ptr<GenericGraph2D>
        Read(const std::string& graphTypeName, std::string graphMagicWord, std::istream& stream, const std::string& fileName, std::string graphName);
}
