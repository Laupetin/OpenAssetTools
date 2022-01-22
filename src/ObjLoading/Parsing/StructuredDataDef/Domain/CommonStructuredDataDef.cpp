#include "CommonStructuredDataDef.h"

CommonStructuredDataDef::CommonStructuredDataDef()
    : CommonStructuredDataDef(0)
{
}

CommonStructuredDataDef::CommonStructuredDataDef(const int version)
    : m_version(version),
      m_checksum(0u),
      m_size_in_byte(0u)
{
}
