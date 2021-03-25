#pragma once
#include <functional>

#include "InfoString/InfoString.h"
#include "Zone/ZoneTypes.h"

class InfoStringFromStructConverterBase
{
protected:
    InfoString m_info_string;
    const void* m_structure;
    const std::function<std::string(scr_string_t)> m_get_scr_string;

    static const char* AssetName(const char* name);
    void FillFromString(const std::string& key, size_t offset);
    void FillFromStringBuffer(const std::string& key, size_t offset, size_t bufferSize);
    void FillFromInt(const std::string& key, size_t offset);
    void FillFromUint(const std::string&, size_t offset);
    void FillFromBool(const std::string&, size_t offset);
    void FillFromQBoolean(const std::string& key, size_t offset);
    void FillFromFloat(const std::string& key, size_t offset);
    void FillFromMilliseconds(const std::string& key, size_t offset);
    void FillFromScriptString(const std::string& key, size_t offset);
    void FillFromEnumInt(const std::string& key, size_t offset, const char** enumValues, size_t enumSize);

    virtual void FillInfoString() = 0;

public:
    explicit InfoStringFromStructConverterBase(const void* structure);
    InfoStringFromStructConverterBase(const void* structure, std::function<std::string(scr_string_t)> scriptStringValueCallback);
    virtual ~InfoStringFromStructConverterBase() = default;
    InfoStringFromStructConverterBase(const InfoStringFromStructConverterBase& other) = delete;
    InfoStringFromStructConverterBase(InfoStringFromStructConverterBase&& other) noexcept = delete;
    InfoStringFromStructConverterBase& operator=(const InfoStringFromStructConverterBase& other) = delete;
    InfoStringFromStructConverterBase& operator=(InfoStringFromStructConverterBase&& other) noexcept = delete;

    InfoString Convert();
};