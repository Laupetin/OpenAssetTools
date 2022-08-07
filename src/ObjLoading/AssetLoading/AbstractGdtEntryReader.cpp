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

Vector2f AbstractGdtEntryReader::ReadVec2Property(const std::string& propertyName, const Vector2f defaultValue) const
{
    const auto foundProperty = m_entry.m_properties.find(propertyName);
    if (foundProperty == m_entry.m_properties.end())
        return defaultValue;

    std::istringstream iss(foundProperty->second);
    Vector2f result;

    iss >> result(0) >> result(1);

    if (iss.fail())
    {
        std::ostringstream oss;
        oss << "\"" << foundProperty->second << "\" is not a valid vec2 value";
        throw GdtReadingException(oss.str());
    }

    return result;
}

Vector3f AbstractGdtEntryReader::ReadVec3Property(const std::string& propertyName, const Vector3f defaultValue) const
{
    const auto foundProperty = m_entry.m_properties.find(propertyName);
    if (foundProperty == m_entry.m_properties.end())
        return defaultValue;

    std::istringstream iss(foundProperty->second);
    Vector3f result;

    iss >> result(0) >> result(1) >> result(2);

    if (iss.fail())
    {
        std::ostringstream oss;
        oss << "\"" << foundProperty->second << "\" is not a valid vec3 value";
        throw GdtReadingException(oss.str());
    }

    return result;
}

Vector4f AbstractGdtEntryReader::ReadVec4Property(const std::string& propertyName, const Vector4f defaultValue) const
{
    const auto foundProperty = m_entry.m_properties.find(propertyName);
    if (foundProperty == m_entry.m_properties.end())
        return defaultValue;

    std::istringstream iss(foundProperty->second);
    Vector4f result;

    iss >> result(0) >> result(1) >> result(2) >> result(3);

    if (iss.fail())
    {
        std::ostringstream oss;
        oss << "\"" << foundProperty->second << "\" is not a valid vec4 value";
        throw GdtReadingException(oss.str());
    }

    return result;
}
