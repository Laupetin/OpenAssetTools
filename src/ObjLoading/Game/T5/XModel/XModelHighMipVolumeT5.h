#pragma once

#include "Game/T5/T5.h"
#include "Utils/MemoryManager.h"

namespace xmodel
{
    T5::XModelHighMipBounds* GenerateHighMipVolumeT5(const T5::XModel& model, MemoryManager& memory);
}
