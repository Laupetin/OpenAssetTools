#include "StepLoadZoneSizes.h"

StepLoadZoneSizes::StepLoadZoneSizes()
    : m_size(0),
      m_external_size(0)
{
}

void StepLoadZoneSizes::PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream)
{
    stream.Load(&m_size, sizeof(m_size));
    stream.Load(&m_external_size, sizeof(m_external_size));
}

size_t StepLoadZoneSizes::GetSize() const
{
    return m_size;
}

size_t StepLoadZoneSizes::GetExternalSize() const
{
    return m_external_size;
}
