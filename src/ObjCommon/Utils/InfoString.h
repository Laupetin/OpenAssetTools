#pragma once
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>


#include "Obj/GDT/GdtEntry.h"
#include "Zone/ZoneTypes.h"

class InfoString
{
    static const std::string EMPTY_VALUE;
    std::unordered_map<std::string, std::string> m_values;
    std::vector<std::string> m_keys_by_insertion;

public:
    bool HasKey(const std::string& key) const;
    const std::string& GetValueForKey(const std::string& key) const;
    const std::string& GetValueForKey(const std::string& key, bool* foundValue) const;
    void SetValueForKey(const std::string& key, std::string value);
    void RemoveKey(const std::string& key);

    std::string ToString() const;
    std::string ToString(const std::string& prefix) const;
    void ToGdtProperties(const std::string& prefix, GdtEntry& gdtEntry) const;

    void FromString();
    void FromString(const std::string& prefix);
};

class InfoStringToStructConverterBase
{
protected:
    const InfoString& m_info_string;
    void* m_structure;

    virtual void FillStructure() = 0;

public:
    InfoStringToStructConverterBase(const InfoString& infoString, void* structure);
    virtual ~InfoStringToStructConverterBase();
    InfoStringToStructConverterBase(const InfoStringToStructConverterBase& other) = delete;
    InfoStringToStructConverterBase(InfoStringToStructConverterBase&& other) noexcept = delete;
    InfoStringToStructConverterBase& operator=(const InfoStringToStructConverterBase& other) = delete;
    InfoStringToStructConverterBase& operator=(InfoStringToStructConverterBase&& other) noexcept = delete;

    void Convert();
};

class InfoStringFromStructConverterBase
{
protected:
    InfoString m_info_string;
    const void* m_structure;
    const std::function<std::string(scr_string_t)> m_get_scr_string;

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
    virtual ~InfoStringFromStructConverterBase();
    InfoStringFromStructConverterBase(const InfoStringFromStructConverterBase& other) = delete;
    InfoStringFromStructConverterBase(InfoStringFromStructConverterBase&& other) noexcept = delete;
    InfoStringFromStructConverterBase& operator=(const InfoStringFromStructConverterBase& other) = delete;
    InfoStringFromStructConverterBase& operator=(InfoStringFromStructConverterBase&& other) noexcept = delete;

    InfoString Convert();
};