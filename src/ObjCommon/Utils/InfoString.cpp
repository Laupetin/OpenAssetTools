#include "InfoString.h"

#include <cassert>
#include <sstream>

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

void InfoString::FromString()
{
}

void InfoString::FromString(const std::string& prefix)
{
}

InfoStringToStructConverterBase::InfoStringToStructConverterBase(const InfoString& infoString, void* structure)
    : m_info_string(infoString),
      m_structure(structure)
{
}

InfoStringToStructConverterBase::~InfoStringToStructConverterBase()
= default;

void InfoStringToStructConverterBase::Convert()
{
    FillStructure();
}

InfoStringFromStructConverterBase::InfoStringFromStructConverterBase(const void* structure)
    : m_structure(structure),
      m_get_scr_string([](scr_string_t)
      {
          assert(false);
          return "";
      })
{
}

InfoStringFromStructConverterBase::InfoStringFromStructConverterBase(const void* structure,
                                                                     std::function<std::string(scr_string_t)>
                                                                     scriptStringValueCallback)
    : m_structure(structure),
      m_get_scr_string(std::move(scriptStringValueCallback))
{
}

InfoStringFromStructConverterBase::~InfoStringFromStructConverterBase()
= default;

InfoString InfoStringFromStructConverterBase::Convert()
{
    FillInfoString();
    return std::move(m_info_string);
}

void InfoStringFromStructConverterBase::FillFromString(const std::string& key, size_t offset)
{
    const auto* str = *reinterpret_cast<const char**>(reinterpret_cast<uintptr_t>(m_structure) + offset);

    if (str)
        m_info_string.SetValueForKey(key, std::string(str));
    else
        m_info_string.SetValueForKey(key, "");
}

void InfoStringFromStructConverterBase::FillFromStringBuffer(const std::string& key, const size_t offset,
                                                             const size_t bufferSize)
{
    const auto* str = reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    const auto strLen = strnlen_s(str, bufferSize);
    m_info_string.SetValueForKey(key, std::string(str, strLen));
}

void InfoStringFromStructConverterBase::FillFromInt(const std::string& key, const size_t offset)
{
    const auto* num = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    m_info_string.SetValueForKey(key, std::to_string(*num));
}

void InfoStringFromStructConverterBase::FillFromUint(const std::string& key, const size_t offset)
{
    const auto* num = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    m_info_string.SetValueForKey(key, std::to_string(*num));
}

void InfoStringFromStructConverterBase::FillFromBool(const std::string& key, const size_t offset)
{
    const auto* bBool = reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    m_info_string.SetValueForKey(key, *bBool ? "1" : "0");
}

void InfoStringFromStructConverterBase::FillFromQBoolean(const std::string& key, const size_t offset)
{
    const auto* iBool = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    m_info_string.SetValueForKey(key, *iBool != 0 ? "1" : "0");
}

void InfoStringFromStructConverterBase::FillFromFloat(const std::string& key, const size_t offset)
{
    const auto* num = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    m_info_string.SetValueForKey(key, std::to_string(*num));
}

void InfoStringFromStructConverterBase::FillFromMilliseconds(const std::string& key, const size_t offset)
{
    const auto* millis = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    m_info_string.SetValueForKey(key, std::to_string(static_cast<float>(*millis) / 1000.0f));
}

void InfoStringFromStructConverterBase::FillFromScriptString(const std::string& key, const size_t offset)
{
    const auto* scrStr = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    m_info_string.SetValueForKey(key, m_get_scr_string(*scrStr));
}

void InfoStringFromStructConverterBase::FillFromEnumInt(const std::string& key, const size_t offset,
                                                        const char** enumValues, const size_t enumSize)
{
    const auto num = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + offset);

    assert(num >= 0 && num < static_cast<int>(enumSize));

    if (num >= 0 && num < static_cast<int>(enumSize))
        m_info_string.SetValueForKey(key, std::string(enumValues[num]));
    else
        m_info_string.SetValueForKey(key, "");
}
