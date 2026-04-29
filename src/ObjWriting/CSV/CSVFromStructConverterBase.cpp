#include "CSVFromStructConverterBase.h"

#include <cassert>
#include <cstring>
#include <sstream>

CSVFromStructConverterBase::CSVFromStructConverterBase(const void* structure)
    : m_structure(structure),
      m_get_scr_string(
          [](scr_string_t)
          {
              assert(false);
              return "";
          })
{
}

CSVFromStructConverterBase::CSVFromStructConverterBase(const void* structure, std::function<std::string(scr_string_t)> scriptStringValueCallback)
    : m_structure(structure),
      m_get_scr_string(std::move(scriptStringValueCallback))
{
}

InfoString CSVFromStructConverterBase::Convert()
{
    FillInfoString();
    return std::move(m_info_string);
}

const char* CSVFromStructConverterBase::AssetName(const char* name)
{
    if (name && name[0] == ',')
        return &name[1];

    return name;
}

void CSVFromStructConverterBase::FillFromString(const std::string& key, const size_t offset)
{
    const auto* str = *reinterpret_cast<const char**>(reinterpret_cast<uintptr_t>(m_structure) + offset);

    if (str)
        m_info_string.SetValueForKey(key, std::string(str));
    else
        m_info_string.SetValueForKey(key, "");
}

void CSVFromStructConverterBase::FillFromStringBuffer(const std::string& key, const size_t offset, const size_t bufferSize)
{
    const auto* str = reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    const auto strLen = strnlen(str, bufferSize);
    m_info_string.SetValueForKey(key, std::string(str, strLen));
}

void CSVFromStructConverterBase::FillFromInt(const std::string& key, const size_t offset)
{
    const auto* num = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    m_info_string.SetValueForKey(key, std::to_string(*num));
}

void CSVFromStructConverterBase::FillFromUint(const std::string& key, const size_t offset)
{
    const auto* num = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    m_info_string.SetValueForKey(key, std::to_string(*num));
}

void CSVFromStructConverterBase::FillFromBool(const std::string& key, const size_t offset)
{
    const auto* bBool = reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    m_info_string.SetValueForKey(key, *bBool ? "1" : "0");
}

void CSVFromStructConverterBase::FillFromQBoolean(const std::string& key, const size_t offset)
{
    const auto* iBool = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    m_info_string.SetValueForKey(key, *iBool != 0 ? "1" : "0");
}

void CSVFromStructConverterBase::FillFromFloat(const std::string& key, const size_t offset)
{
    const auto* num = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_structure) + offset);

    std::ostringstream ss;
    ss << *num;

    m_info_string.SetValueForKey(key, ss.str());
}

void CSVFromStructConverterBase::FillFromMilliseconds(const std::string& key, const size_t offset)
{
    const auto* millis = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_structure) + offset);

    const auto value = static_cast<float>(*millis) / 1000.0f;

    std::ostringstream ss;
    ss << value;

    m_info_string.SetValueForKey(key, ss.str());
}

void CSVFromStructConverterBase::FillFromScriptString(const std::string& key, const size_t offset)
{
    const auto* scrStr = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + offset);
    m_info_string.SetValueForKey(key, m_get_scr_string(*scrStr));
}

void CSVFromStructConverterBase::FillFromEnumInt(const std::string& key, const size_t offset, const char** enumValues, const size_t enumSize)
{
    const auto num = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + offset);

    assert(num >= 0 && num < static_cast<int>(enumSize));

    if (num >= 0 && num < static_cast<int>(enumSize))
        m_info_string.SetValueForKey(key, std::string(enumValues[num]));
    else
        m_info_string.SetValueForKey(key, "");
}
