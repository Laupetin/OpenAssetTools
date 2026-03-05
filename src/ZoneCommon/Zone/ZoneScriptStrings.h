#pragma once

#include "Zone/ZoneTypes.h"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

class ZoneScriptStrings
{
    int m_null_entry_pos;
    std::vector<std::string> m_scr_strings;
    std::unordered_map<std::string, scr_string_t> m_scr_string_lookup;

public:
    ZoneScriptStrings();

    void InitializeForExistingZone();
    void InitializeForExistingZone(const char** scrStrList, size_t scrStrCount);

    void AddScriptString(const char* value);
    void AddScriptString(const std::string& value);
    scr_string_t AddOrGetScriptString(const char* value);
    scr_string_t AddOrGetScriptString(const std::string& value);
    [[nodiscard]] scr_string_t GetScriptString(const char* value) const;
    [[nodiscard]] scr_string_t GetScriptString(const std::string& value) const;

    [[nodiscard]] size_t Count() const;
    [[nodiscard]] bool Empty() const;

    [[nodiscard]] const char* CValue(size_t index) const;
    [[nodiscard]] const std::string& Value(size_t index) const;
    [[nodiscard]] const std::string& Value(size_t index, bool& isNull) const;
    [[nodiscard]] const std::string& operator[](size_t index) const;
    [[nodiscard]] std::vector<std::string>::const_iterator begin() const;
    [[nodiscard]] std::vector<std::string>::const_iterator end() const;
};
