#include "InfoStringToStructConverterBase.h"

#include <cstring>
#include <format>
#include <iostream>
#include <sstream>

InfoStringToStructConverterBase::InfoStringToStructConverterBase(const InfoString& infoString,
                                                                 void* structure,
                                                                 ZoneScriptStrings& zoneScriptStrings,
                                                                 MemoryManager& memory,
                                                                 AssetCreationContext& context,
                                                                 GenericAssetRegistration& registration)
    : m_info_string(infoString),
      m_structure(structure),
      m_zone_script_strings(zoneScriptStrings),
      m_memory(memory),
      m_context(context),
      m_registration(registration)
{
}

bool InfoStringToStructConverterBase::ParseAsArray(const std::string& value, std::vector<std::string>& valueArray)
{
    auto startPos = 0u;
    for (auto ci = 0u; ci < value.size(); ci++)
    {
        const auto c = value[ci];

        if (c == '\r' && ci + 1 < value.size() && value[ci + 1] == '\n')
        {
            valueArray.emplace_back(value, startPos, ci - startPos);
            startPos = ++ci + 1;
        }
        else if (c == '\n')
        {
            valueArray.emplace_back(value, startPos, ci - startPos);
            startPos = ci + 1;
        }
    }

    if (startPos < value.size())
    {
        valueArray.emplace_back(value, startPos, value.size() - startPos);
    }

    return true;
}

bool InfoStringToStructConverterBase::ConvertString(const std::string& value, const size_t offset)
{
    *reinterpret_cast<const char**>(reinterpret_cast<uintptr_t>(m_structure) + offset) = m_memory.Dup(value.c_str());
    return true;
}

bool InfoStringToStructConverterBase::ConvertStringBuffer(const std::string& value, const size_t offset, const size_t bufferSize)
{
    strncpy(reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(m_structure) + offset), value.c_str(), bufferSize);
    return true;
}

bool InfoStringToStructConverterBase::ConvertInt(const std::string& value, const size_t offset)
{
    char* endPtr;
    *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + offset) = strtol(value.c_str(), &endPtr, 0);

    if (endPtr != &value[value.size()])
    {
        std::cerr << std::format("Failed to parse value \"{}\" as int\n", value);
        return false;
    }

    return true;
}

bool InfoStringToStructConverterBase::ConvertUint(const std::string& value, const size_t offset)
{
    char* endPtr;
    *reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_structure) + offset) = strtoul(value.c_str(), &endPtr, 0);

    if (endPtr != &value[value.size()])
    {
        std::cerr << std::format("Failed to parse value \"{}\" as uint\n", value);
        return false;
    }

    return true;
}

bool InfoStringToStructConverterBase::ConvertBool(const std::string& value, const size_t offset)
{
    char* endPtr;
    const auto intValue = strtol(value.c_str(), &endPtr, 0);

    *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(m_structure) + offset) = intValue != 0;
    if (endPtr != &value[value.size()])
    {
        std::cerr << std::format("Failed to parse value \"{}\" as bool\n", value);
        return false;
    }

    return true;
}

bool InfoStringToStructConverterBase::ConvertQBoolean(const std::string& value, const size_t offset)
{
    char* endPtr;
    const auto intValue = strtol(value.c_str(), &endPtr, 0);

    *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + offset) = intValue != 0 ? 1 : 0;
    if (endPtr != &value[value.size()])
    {
        std::cerr << std::format("Failed to parse value \"{}\" as qboolean\n", value);
        return false;
    }

    return true;
}

bool InfoStringToStructConverterBase::ConvertFloat(const std::string& value, const size_t offset)
{
    char* endPtr;
    *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_structure) + offset) = strtof(value.c_str(), &endPtr);

    if (endPtr != &value[value.size()])
    {
        std::cerr << std::format("Failed to parse value \"{}\" as float\n", value);
        return false;
    }

    return true;
}

bool InfoStringToStructConverterBase::ConvertMilliseconds(const std::string& value, const size_t offset)
{
    char* endPtr;
    *reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_structure) + offset) = static_cast<unsigned int>(strtof(value.c_str(), &endPtr) * 1000.0f);

    if (endPtr != &value[value.size()])
    {
        std::cerr << std::format("Failed to parse value \"{}\" as milliseconds\n", value);
        return false;
    }

    return true;
}

bool InfoStringToStructConverterBase::ConvertScriptString(const std::string& value, const size_t offset)
{
    auto scrStrValue = m_zone_script_strings.AddOrGetScriptString(value);
    m_registration.AddScriptString(scrStrValue);
    *reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + offset) = scrStrValue;

    return true;
}

bool InfoStringToStructConverterBase::ConvertEnumInt(
    const std::string& fieldName, const std::string& value, const size_t offset, const char** enumValues, const size_t enumSize)
{
    for (auto i = 0u; i < enumSize; i++)
    {
        if (value == enumValues[i])
        {
            *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + offset) = static_cast<int>(i);
            return true;
        }
    }

    std::ostringstream ss;
    ss << "Not a valid value for field \"" << fieldName << "\": \"" << value << "\". Valid values are:\n    ";

    for (auto i = 0u; i < enumSize; i++)
    {
        if (i > 0)
            ss << ", ";
        ss << '"' << enumValues[i] << '"';
    }

    ss << '\n';
    std::cerr << ss.str();

    return false;
}
