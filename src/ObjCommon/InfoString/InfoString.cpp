#include "InfoString.h"

#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <stack>

namespace
{
    constexpr const char* GDT_PREFIX_FIELD = "configstringFileType";
    const std::string EMPTY_VALUE;
} // namespace

bool InfoString::HasKey(const std::string& key) const
{
    return m_value_lookup.contains(key);
}

const std::string& InfoString::GetValueForKey(const std::string& key) const
{
    const auto& value = m_value_lookup.find(key);

    if (value == m_value_lookup.end())
        return EMPTY_VALUE;

    return value->second;
}

const std::string& InfoString::GetValueForKey(const std::string& key, bool* foundValue) const
{
    const auto& value = m_value_lookup.find(key);

    if (value == m_value_lookup.end())
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
        m_keys_by_insertion.emplace_back(key);

    m_value_lookup[key] = std::move(value);
}

void InfoString::RemoveKey(const std::string& key)
{
    const auto& value = m_value_lookup.find(key);

    if (value != m_value_lookup.end())
        m_value_lookup.erase(value);

    const auto insertion = std::ranges::find_if(m_keys_by_insertion,
                                                [&key](const std::string& keyByInsertion)
                                                {
                                                    return utils::StringEqualsIgnoreCase(key, keyByInsertion);
                                                });
}

std::string InfoString::ToString() const
{
    std::stringstream ss;
    bool first = true;

    for (const auto& key : m_keys_by_insertion)
    {
        const auto value = m_value_lookup.find(key);
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
        const auto value = m_value_lookup.find(key);
        ss << '\\' << key << '\\' << value->second;
    }

    return ss.str();
}

void InfoString::ToGdtProperties(const std::string& prefix, GdtEntry& gdtEntry) const
{
    for (const auto& key : m_keys_by_insertion)
    {
        const auto value = m_value_lookup.find(key);
        gdtEntry.m_properties[key] = value->second;
    }

    gdtEntry.m_properties[GDT_PREFIX_FIELD] = prefix;
}

class InfoStringInputStream
{
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

private:
    std::istream& m_stream;
    int m_last_separator;
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

        const auto existingEntry = m_value_lookup.find(key);
        if (existingEntry == m_value_lookup.end())
        {
            m_keys_by_insertion.push_back(key);
            m_value_lookup.emplace(std::make_pair(key, value));
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
    {
        con::error("Invalid info string: Empty");
        return false;
    }

    if (prefix != readPrefix)
    {
        con::error(R"(Invalid info string: Prefix "{}" did not match expected prefix "{}")", readPrefix, prefix);
        return false;
    }

    std::string key;
    while (infoStream.NextField(key))
    {
        if (key.empty())
        {
            if (m_keys_by_insertion.empty())
                con::error("Invalid info string: Got empty key at the start of the info string");
            else
                con::error("Invalid info string: Got empty key after key \"{}\"", m_keys_by_insertion[m_keys_by_insertion.size() - 1]);

            return false;
        }

        std::string value;
        if (!infoStream.NextField(value))
        {
            con::error("Invalid info string: Unexpected eof, no value for key \"{}\"", key);
            return false;
        }

        const auto existingEntry = m_value_lookup.find(key);
        if (existingEntry == m_value_lookup.end())
        {
            m_keys_by_insertion.push_back(key);
            m_value_lookup.emplace(std::make_pair(key, value));
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
            auto existingEntry = m_value_lookup.find(key);
            if (existingEntry == m_value_lookup.end())
            {
                m_keys_by_insertion.push_back(key);
                m_value_lookup.emplace(std::make_pair(key, value));
            }
            else
            {
                existingEntry->second = value;
            }
        }
    }

    return true;
}
