#include "StepWriteTimestamp.h"

#include <algorithm>
#include <ctime>

StepWriteTimestamp::StepWriteTimestamp() {}

void StepWriteTimestamp::PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream)
{
    // time_t is not guaranteed to be 64bit so cast is necessary
    const auto time = static_cast<int64_t>(std::time(nullptr));
    stream->Write(&time, sizeof(time));
}
