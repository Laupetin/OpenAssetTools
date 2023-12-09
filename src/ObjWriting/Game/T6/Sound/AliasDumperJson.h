#pragma once

#include "AbstractAliasDumper.h"
#include "Dumping/AssetDumpingContext.h"

#include <memory>

namespace T6::sound
{
    std::unique_ptr<AbstractAliasDumper> CreateAliasDumperJson(const AssetDumpingContext& context);
}
