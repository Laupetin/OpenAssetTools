#pragma once

#include "Loading/StreamProcessor.h"

#include <memory>

namespace processor
{
    std::unique_ptr<StreamProcessor> CreateProcessorIW4xDecryption();
}
