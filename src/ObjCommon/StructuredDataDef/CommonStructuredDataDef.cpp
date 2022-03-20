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

uint32_t CommonStructuredDataDef::CalculateChecksum() const
{
    auto checksum = 0u;
    for (const auto& _enum : m_enums)
        checksum = _enum->CalculateChecksum(checksum);
    for (const auto& _struct : m_structs)
        checksum = _struct->CalculateChecksum(*this, checksum);

    return checksum;
}
