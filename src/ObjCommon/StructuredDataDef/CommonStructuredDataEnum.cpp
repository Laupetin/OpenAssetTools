#include "CommonStructuredDataEnum.h"

#include "Utils/Endianness.h"

#include <algorithm>
#include <zlib.h>

CommonStructuredDataEnumEntry::CommonStructuredDataEnumEntry()
    : m_value(0u)
{
}

CommonStructuredDataEnumEntry::CommonStructuredDataEnumEntry(std::string name, const size_t value)
    : m_name(std::move(name)),
      m_value(value)
{
}

CommonStructuredDataEnum::CommonStructuredDataEnum()
    : m_reserved_entry_count(-1)
{
}

CommonStructuredDataEnum::CommonStructuredDataEnum(std::string name)
    : m_name(std::move(name)),
      m_reserved_entry_count(-1)
{
}

CommonStructuredDataEnum::CommonStructuredDataEnum(std::string name, const int reservedEntryCount)
    : m_name(std::move(name)),
      m_reserved_entry_count(reservedEntryCount)
{
}

size_t CommonStructuredDataEnum::ElementCount() const
{
    return m_reserved_entry_count > 0 ? static_cast<size_t>(m_reserved_entry_count) : m_entries.size();
}

uint32_t CommonStructuredDataEnum::CalculateChecksum(const uint32_t initialValue) const
{
    auto checksum = initialValue;

    checksum = crc32(checksum, reinterpret_cast<const Bytef*>(m_name.c_str()), static_cast<unsigned>(m_name.size() + 1u));

    const auto littleEndianElementCount = endianness::ToLittleEndian(ElementCount());
    checksum = crc32(checksum, reinterpret_cast<const Bytef*>(&littleEndianElementCount), sizeof(littleEndianElementCount));

    for (const auto& entry : m_entries)
    {
        checksum = crc32(checksum, reinterpret_cast<const Bytef*>(entry.m_name.c_str()), static_cast<unsigned>(entry.m_name.size() + 1));

        const auto littleEndianValue = endianness::ToLittleEndian(entry.m_value);
        checksum = crc32(checksum, reinterpret_cast<const Bytef*>(&littleEndianValue), sizeof(littleEndianValue));
    }

    return checksum;
}

void CommonStructuredDataEnum::SortEntriesByOffset()
{
    std::ranges::sort(m_entries,
                      [](const CommonStructuredDataEnumEntry& e1, const CommonStructuredDataEnumEntry& e2)
                      {
                          return e1.m_value < e2.m_value;
                      });
}

void CommonStructuredDataEnum::SortEntriesByName()
{
    std::ranges::sort(m_entries,
                      [](const CommonStructuredDataEnumEntry& e1, const CommonStructuredDataEnumEntry& e2)
                      {
                          return e1.m_name < e2.m_name;
                      });
}
