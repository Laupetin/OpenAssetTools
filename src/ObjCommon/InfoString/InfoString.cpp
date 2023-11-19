#include "InfoString.h"

#include <cstring>
#include <sstream>
#include <stack>

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

    gdtEntry.m_properties[GDT_PREFIX_FIELD] = prefix;
}

class InfoStringInputStream
{
    std::istream& m_stream;
    int m_last_separator;

public:
    explicit InfoStringInputStream(std::istream& stream)
        : m_stream(stream),
          m_last_separator(EOF)
    {
    }

    bool NextField(std::string& value)
    {
        std::ostringstream str;

        auto c = m_stream.get();
        if (c == EOF)
        {
            if (m_last_separator != EOF)
            {
                m_last_separator = EOF;
                value = std::string();
                return true;
            }

            return false;
        }

        while (c != EOF && c != '\\')
        {
            str << static_cast<char>(c);
            c = m_stream.get();
        }

        m_last_separator = c;
        value = str.str();
        return true;
    }
};

bool InfoString::FromStream(std::istream& stream)
{
    InfoStringInputStream infoStream(stream);

    std::string key;
    while (infoStream.NextField(key))
    {
        std::string value;
        if (!infoStream.NextField(value))
            return false;

        const auto existingEntry = m_values.find(key);
        if (existingEntry == m_values.end())
        {
            m_keys_by_insertion.push_back(key);
            m_values.emplace(std::make_pair(key, value));
        }
        else
        {
            existingEntry->second = value;
        }
    }

    return true;
}

bool InfoString::FromStream(const std::string& prefix, std::istream& stream)
{
    InfoStringInputStream infoStream(stream);

    std::string readPrefix;
    if (!infoStream.NextField(readPrefix))
        return false;

    if (prefix != readPrefix)
        return false;

    std::string key;
    while (infoStream.NextField(key))
    {
        std::string value;
        if (!infoStream.NextField(value))
            return false;

        const auto existingEntry = m_values.find(key);
        if (existingEntry == m_values.end())
        {
            m_keys_by_insertion.push_back(key);
            m_values.emplace(std::make_pair(key, value));
        }
        else
        {
            existingEntry->second = value;
        }
    }

    return true;
}

bool InfoString::FromGdtProperties(const GdtEntry& gdtEntry)
{
    std::stack<const GdtEntry*> entryStack;

    {
        const auto* currentEntry = &gdtEntry;
        while (currentEntry)
        {
            entryStack.push(currentEntry);
            currentEntry = currentEntry->m_parent;
        }
    }

    while (!entryStack.empty())
    {
        const auto* currentEntry = entryStack.top();
        entryStack.pop();

        for (const auto& [key, value] : currentEntry->m_properties)
        {
            auto existingEntry = m_values.find(key);
            if (existingEntry == m_values.end())
            {
                m_keys_by_insertion.push_back(key);
                m_values.emplace(std::make_pair(key, value));
            }
            else
            {
                existingEntry->second = value;
            }
        }
    }

    return true;
}
