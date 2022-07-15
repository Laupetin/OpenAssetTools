#include "AbstractGdtEntryReader.h"

#include <sstream>

GdtReadingException::GdtReadingException(std::string message)
    : exception(message.c_str()),
      m_message(std::move(message))
{
}

const char* GdtReadingException::what() const
{
    return m_message.c_str();
}

AbstractGdtEntryReader::AbstractGdtEntryReader(const GdtEntry& entry)
    : m_entry(entry)
{
}

std::string AbstractGdtEntryReader::ReadStringProperty(const std::string& propertyName, std::string defaultValue) const
{
    const auto foundProperty = m_entry.m_properties.find(propertyName);
    if (foundProperty == m_entry.m_properties.end())
        return defaultValue;

    return foundProperty->second;
}

bool AbstractGdtEntryReader::ReadBoolProperty(const std::string& propertyName, const bool defaultValue) const
{
    const auto foundProperty = m_entry.m_properties.find(propertyName);
    if (foundProperty == m_entry.m_properties.end())
        return defaultValue;

    char* endPtr = nullptr;
    const auto result = std::strtol(foundProperty->second.c_str(), &endPtr, 10);
    if (endPtr != &foundProperty->second[foundProperty->second.size()])
    {
        std::ostringstream ss;
        ss << "\"" << foundProperty->second << "\" is not a valid boolean value";
        throw GdtReadingException(ss.str());
    }

    return result != 0;
}

int AbstractGdtEntryReader::ReadIntegerProperty(const std::string& propertyName, const int defaultValue) const
{
    const auto foundProperty = m_entry.m_properties.find(propertyName);
    if (foundProperty == m_entry.m_properties.end())
        return defaultValue;

    char* endPtr = nullptr;
    const auto result = std::strtol(foundProperty->second.c_str(), &endPtr, 10);
    if (endPtr != &foundProperty->second[foundProperty->second.size()])
    {
        std::ostringstream ss;
        ss << "\"" << foundProperty->second << "\" is not a valid integer value";
        throw GdtReadingException(ss.str());
    }

    return result;
}

float AbstractGdtEntryReader::ReadFloatProperty(const std::string& propertyName, const float defaultValue) const
{
    const auto foundProperty = m_entry.m_properties.find(propertyName);
    if (foundProperty == m_entry.m_properties.end())
        return defaultValue;

    char* endPtr = nullptr;
    const auto result = std::strtof(foundProperty->second.c_str(), &endPtr);
    if (endPtr != &foundProperty->second[foundProperty->second.size()])
    {
        std::ostringstream ss;
        ss << "\"" << foundProperty->second << "\" is not a valid float value";
        throw GdtReadingException(ss.str());
    }

    return result;
}
