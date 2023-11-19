#pragma once

#include "InfoString/InfoString.h"
#include "Pool/XAssetInfo.h"
#include "Utils/ClassUtils.h"
#include "Utils/MemoryManager.h"
#include "Zone/ZoneScriptStrings.h"

#include <string>
#include <unordered_set>
#include <utility>

class InfoStringToStructConverterBase
{
protected:
    const InfoString& m_info_string;
    ZoneScriptStrings& m_zone_script_strings;
    std::unordered_set<scr_string_t> m_used_script_string_list;
    std::unordered_set<XAssetInfoGeneric*> m_dependencies;
    MemoryManager* m_memory;
    void* m_structure;

    static bool ParseAsArray(const std::string& value, std::vector<std::string>& valueArray);
    bool ParseAsPairs(const std::string& value, std::vector<std::pair<std::string, std::string>>& valueArray) const;

    bool ConvertString(const std::string& value, size_t offset);
    bool ConvertStringBuffer(const std::string& value, size_t offset, size_t bufferSize);
    bool ConvertInt(const std::string& value, size_t offset);
    bool ConvertUint(const std::string& value, size_t offset);
    bool ConvertBool(const std::string& value, size_t offset);
    bool ConvertQBoolean(const std::string& value, size_t offset);
    bool ConvertFloat(const std::string& value, size_t offset);
    bool ConvertMilliseconds(const std::string& value, size_t offset);
    bool ConvertScriptString(const std::string& value, size_t offset);
    bool ConvertEnumInt(const std::string& value, size_t offset, const char** enumValues, size_t enumSize);

public:
    InfoStringToStructConverterBase(const InfoString& infoString, void* structure, ZoneScriptStrings& zoneScriptStrings, MemoryManager* memory);
    virtual ~InfoStringToStructConverterBase() = default;
    InfoStringToStructConverterBase(const InfoStringToStructConverterBase& other) = delete;
    InfoStringToStructConverterBase(InfoStringToStructConverterBase&& other) noexcept = delete;
    InfoStringToStructConverterBase& operator=(const InfoStringToStructConverterBase& other) = delete;
    InfoStringToStructConverterBase& operator=(InfoStringToStructConverterBase&& other) noexcept = delete;

    virtual bool Convert() = 0;
    _NODISCARD std::vector<scr_string_t> GetUsedScriptStrings() const;
    _NODISCARD std::vector<XAssetInfoGeneric*> GetDependencies() const;
};
