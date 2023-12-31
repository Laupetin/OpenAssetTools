#include "CommonLocalizeEntry.h"

CommonLocalizeEntry::CommonLocalizeEntry() = default;

CommonLocalizeEntry::CommonLocalizeEntry(std::string key, std::string value)
    : m_key(std::move(key)),
      m_value(std::move(value))
{
}
