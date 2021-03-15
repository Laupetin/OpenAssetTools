#include "StepWriteZoneContentToFile.h"

StepWriteZoneContentToFile::StepWriteZoneContentToFile(StepWriteZoneContentToMemory* zoneMemory)
    : m_memory(zoneMemory)
{
}

void StepWriteZoneContentToFile::PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream)
{
}
