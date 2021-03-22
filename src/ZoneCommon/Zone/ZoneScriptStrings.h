#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "Utils/ClassUtils.h"
#include "Zone/ZoneTypes.h"

class ZoneScriptStrings
{
    std::vector<std::string> m_scr_strings;
    std::unordered_map<std::string, scr_string_t> m_scr_string_lookup;

public:
    void AddScriptString(const std::string& value);
    scr_string_t AddOrGetScriptString(const std::string& value);
    _NODISCARD scr_string_t GetScriptString(const std::string& value) const;
    _NODISCARD size_t Count() const;
    _NODISCARD bool Empty() const;

    _NODISCARD const std::string& operator[](size_t index) const;
    _NODISCARD std::vector<std::string>::const_iterator begin() const;
    _NODISCARD std::vector<std::string>::const_iterator end() const;
};