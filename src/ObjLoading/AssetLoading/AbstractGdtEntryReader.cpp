#include "AbstractGdtEntryReader.h"

#include <sstream>

GdtReadingException::GdtReadingException(std::string message)
    : m_message(std::move(message))
{
}

const char* GdtReadingException::what() const noexcept
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

GdtVec2 AbstractGdtEntryReader::ReadVec2Property(const std::string& propertyName, const GdtVec2 defaultValue) const
{
    const auto foundProperty = m_entry.m_properties.find(propertyName);
    if (foundProperty == m_entry.m_properties.end())
        return defaultValue;

    std::istringstream iss(foundProperty->second);
    GdtVec2 result;

    iss >> result.x >> result.y;

    if (iss.fail())
    {
        std::ostringstream oss;
        oss << "\"" << foundProperty->second << "\" is not a valid vec2 value";
        throw GdtReadingException(oss.str());
    }

    return result;
}

GdtVec3 AbstractGdtEntryReader::ReadVec3Property(const std::string& propertyName, const GdtVec3 defaultValue) const
{
    const auto foundProperty = m_entry.m_properties.find(propertyName);
    if (foundProperty == m_entry.m_properties.end())
        return defaultValue;

    std::istringstream iss(foundProperty->second);
    GdtVec3 result;

    iss >> result.x >> result.y >> result.z;

    if (iss.fail())
    {
        std::ostringstream oss;
        oss << "\"" << foundProperty->second << "\" is not a valid vec3 value";
        throw GdtReadingException(oss.str());
    }

    return result;
}

GdtVec4 AbstractGdtEntryReader::ReadVec4Property(const std::string& propertyName, const GdtVec4 defaultValue) const
{
    const auto foundProperty = m_entry.m_properties.find(propertyName);
    if (foundProperty == m_entry.m_properties.end())
        return defaultValue;

    std::istringstream iss(foundProperty->second);
    GdtVec4 result;

    iss >> result.x >> result.y >> result.z >> result.w;

    if (iss.fail())
    {
        std::ostringstream oss;
        oss << "\"" << foundProperty->second << "\" is not a valid vec4 value";
        throw GdtReadingException(oss.str());
    }

    return result;
}
