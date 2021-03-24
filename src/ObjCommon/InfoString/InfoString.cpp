#include "InfoString.h"

#include <sstream>
#include <cstring>

const std::string InfoString::EMPTY_VALUE;

bool InfoString::HasKey(const std::string& key) const
{
    return m_values.find(key) != m_values.end();
}

const std::string& InfoString::GetValueForKey(const std::string& key) const
{
    const auto& value = m_values.find(key);

    if (value == m_values.end())
        return EMPTY_VALUE;

    return value->second;
}

const std::string& InfoString::GetValueForKey(const std::string& key, bool* foundValue) const
{
    const auto& value = m_values.find(key);

    if (value == m_values.end())
    {
        if (foundValue)
            *foundValue = false;
        return EMPTY_VALUE;
    }

    if (foundValue)
        *foundValue = true;
    return value->second;
}

void InfoString::SetValueForKey(const std::string& key, std::string value)
{
    if (!HasKey(key))
        m_keys_by_insertion.push_back(key);

    m_values[key] = std::move(value);
}

void InfoString::RemoveKey(const std::string& key)
{
    const auto& value = m_values.find(key);

    if (value != m_values.end())
        m_values.erase(value);
}

std::string InfoString::ToString() const
{
    std::stringstream ss;
    bool first = true;

    for (const auto& key : m_keys_by_insertion)
    {
        const auto value = m_values.find(key);
        if (!first)
            ss << '\\';
        else
            first = false;

        ss << key << '\\' << value->second;
    }

    return ss.str();
}

std::string InfoString::ToString(const std::string& prefix) const
{
    std::stringstream ss;
    ss << prefix;

    for (const auto& key : m_keys_by_insertion)
    {
        const auto value = m_values.find(key);
        ss << '\\' << key << '\\' << value->second;
    }

    return ss.str();
}

void InfoString::ToGdtProperties(const std::string& prefix, GdtEntry& gdtEntry) const
{
    for (const auto& key : m_keys_by_insertion)
    {
        const auto value = m_values.find(key);
        gdtEntry.m_properties[key] = value->second;
    }

    gdtEntry.m_properties["configstringFileType"] = prefix;
}

void InfoString::FromString()
{
}

void InfoString::FromString(const std::string& prefix)
{
}
