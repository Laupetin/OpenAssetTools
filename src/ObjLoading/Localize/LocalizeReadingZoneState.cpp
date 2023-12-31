#include "LocalizeReadingZoneState.h"

bool LocalizeReadingZoneState::DoLocalizeEntryDuplicateCheck(const std::string& key)
{
    const auto existingEntry = m_keys.find(key);
    if (existingEntry != m_keys.end())
        return false;

    m_keys.emplace(key);
    return true;
}
