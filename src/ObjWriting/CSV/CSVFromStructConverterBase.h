#pragma once
#include "InfoString/InfoString.h"
#include "Zone/ZoneTypes.h"

#include <functional>

class CSVFromStructConverterBase
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
    explicit CSVFromStructConverterBase(const void* structure);
    CSVFromStructConverterBase(const void* structure, std::function<std::string(scr_string_t)> scriptStringValueCallback);
    virtual ~CSVFromStructConverterBase() = default;
    CSVFromStructConverterBase(const CSVFromStructConverterBase& other) = delete;
    CSVFromStructConverterBase(CSVFromStructConverterBase&& other) noexcept = delete;
    CSVFromStructConverterBase& operator=(const CSVFromStructConverterBase& other) = delete;
    CSVFromStructConverterBase& operator=(CSVFromStructConverterBase&& other) noexcept = delete;

    InfoString Convert();
};
