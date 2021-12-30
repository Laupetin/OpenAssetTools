#include "ZoneScriptStrings.h"

#include <cassert>
#include <stdexcept>
#include <sstream>

ZoneScriptStrings::ZoneScriptStrings()
    : m_null_entry_pos(0)
{
    // Make script string 0 a nullptr string
    m_scr_strings.emplace_back();
}

void ZoneScriptStrings::InitializeForExistingZone()
{
    m_null_entry_pos = -1;
    m_scr_strings.clear();
}

void ZoneScriptStrings::InitializeForExistingZone(const char** scrStrList, const size_t scrStrCount)
{
    InitializeForExistingZone();

    if (!scrStrList)
        return;

    for (auto i = 0u; i < scrStrCount; i++)
        AddScriptString(scrStrList[i]);
}

void ZoneScriptStrings::AddScriptString(const char* value)
{
    const auto newStringIndex = m_scr_strings.size();
    const auto newScrStringIndex = static_cast<scr_string_t>(newStringIndex);

    if (value != nullptr)
    {
        m_scr_strings.emplace_back(value);
        m_scr_string_lookup[value] = newScrStringIndex;
    }
    else
    {
        assert(m_null_entry_pos < 0); // If null index is already set, the previous cost will not be considered null string anymore.
        m_scr_strings.emplace_back();
        m_null_entry_pos = static_cast<int>(newStringIndex);
    }
}

void ZoneScriptStrings::AddScriptString(const std::string& value)
{
    const auto newScrStringIndex = static_cast<scr_string_t>(m_scr_strings.size());
    m_scr_strings.emplace_back(value);
    m_scr_string_lookup[value] = newScrStringIndex;
}

scr_string_t ZoneScriptStrings::AddOrGetScriptString(const char* value)
{
    if (value != nullptr)
    {
        const auto existingScriptString = m_scr_string_lookup.find(value);
        if (existingScriptString != m_scr_string_lookup.end())
            return existingScriptString->second;

        const auto newScrStringIndex = static_cast<scr_string_t>(m_scr_strings.size());
        m_scr_strings.emplace_back(value);
        m_scr_string_lookup[value] = newScrStringIndex;
        return newScrStringIndex;
    }

    if (m_null_entry_pos < 0)
    {
        const auto newStringIndex = m_scr_strings.size();
        m_scr_strings.emplace_back();
        m_null_entry_pos = static_cast<int>(newStringIndex);
        return static_cast<scr_string_t>(newStringIndex);
    }
    return static_cast<scr_string_t>(m_null_entry_pos);
}

scr_string_t ZoneScriptStrings::AddOrGetScriptString(const std::string& value)
{
    const auto existingScriptString = m_scr_string_lookup.find(value);
    if (existingScriptString != m_scr_string_lookup.end())
        return existingScriptString->second;

    const auto newScrStringIndex = static_cast<scr_string_t>(m_scr_strings.size());
    m_scr_strings.emplace_back(value);
    m_scr_string_lookup[value] = newScrStringIndex;

    return newScrStringIndex;
}

scr_string_t ZoneScriptStrings::GetScriptString(const char* value) const
{
    if (value == nullptr)
    {
        if (m_null_entry_pos >= 0)
            return static_cast<scr_string_t>(m_null_entry_pos);

        std::ostringstream str;
        str << "Unable to find script string nullptr";
        throw std::runtime_error(str.str());
    }

    const auto existingScriptString = m_scr_string_lookup.find(value);
    if (existingScriptString != m_scr_string_lookup.end())
        return existingScriptString->second;

    std::ostringstream str;
    str << "Unable to find script string \"" << value << "\"";
    throw std::runtime_error(str.str());
}

scr_string_t ZoneScriptStrings::GetScriptString(const std::string& value) const
{
    const auto existingScriptString = m_scr_string_lookup.find(value);
    if (existingScriptString != m_scr_string_lookup.end())
        return existingScriptString->second;

    std::ostringstream str;
    str << "Unable to find script string \"" << value << "\"";
    throw std::runtime_error(str.str());
}

size_t ZoneScriptStrings::Count() const
{
    return m_scr_strings.size();
}

bool ZoneScriptStrings::Empty() const
{
    return m_scr_strings.empty();
}

const char* ZoneScriptStrings::CValue(const size_t index) const
{
    if (index > m_scr_strings.size())
    {
        std::ostringstream str;
        str << "Script string index '" << index << "' is not inside range of zone script strings (count: " << m_scr_strings.size() << ")";
        throw std::runtime_error(str.str());
    }

    if (m_null_entry_pos == static_cast<int>(index))
        return nullptr;

    return m_scr_strings[index].c_str();
}

const std::string& ZoneScriptStrings::Value(const size_t index) const
{
    return (*this)[index];
}

const std::string& ZoneScriptStrings::Value(const size_t index, bool& isNull) const
{
    if (index > m_scr_strings.size())
    {
        std::ostringstream str;
        str << "Script string index '" << index << "' is not inside range of zone script strings (count: " << m_scr_strings.size() << ")";
        throw std::runtime_error(str.str());
    }

    isNull = m_null_entry_pos == static_cast<int>(index);
    return m_scr_strings[index];
}

const std::string& ZoneScriptStrings::operator[](const size_t index) const
{
    if (index > m_scr_strings.size())
    {
        std::ostringstream str;
        str << "Script string index '" << index << "' is not inside range of zone script strings (count: " << m_scr_strings.size() << ")";
        throw std::runtime_error(str.str());
    }

    return m_scr_strings[index];
}

std::vector<std::string>::const_iterator ZoneScriptStrings::begin() const
{
    return m_scr_strings.cbegin();
}

std::vector<std::string>::const_iterator ZoneScriptStrings::end() const
{
    return m_scr_strings.end();
}
