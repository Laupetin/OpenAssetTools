#include "StepWriteZoneHeader.h"

StepWriteZoneHeader::StepWriteZoneHeader(const ZoneHeader header)
    : m_header(header)
{
}

void StepWriteZoneHeader::PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream)
{
    stream->Write(&m_header, sizeof(m_header));
}
