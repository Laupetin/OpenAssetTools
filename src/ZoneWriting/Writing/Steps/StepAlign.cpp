#include "StepAlign.h"

StepAlign::StepAlign(const size_t alignTo, const uint8_t alignValue)
    : m_align_to(alignTo),
      m_align_value(alignValue)
{
}

void StepAlign::PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream)
{
    const auto pos = stream->Pos();
    const auto targetPos = (pos + m_align_to - 1) / m_align_to * m_align_to;
    const auto valueCount = static_cast<size_t>(targetPos - pos);

    for(auto i = 0u; i < valueCount; i++)
        stream->Write(&m_align_value, sizeof(m_align_value));
}
