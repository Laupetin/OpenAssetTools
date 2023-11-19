#include "LocalizeFile.h"

LocalizeFileEntry::LocalizeFileEntry() = default;

LocalizeFileEntry::LocalizeFileEntry(std::string key, std::string value)
    : m_key(std::move(key)),
      m_value(std::move(value))
{
}
