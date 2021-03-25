#pragma once
#include <istream>
#include <unordered_map>
#include <string>
#include <vector>

#include "Utils/ClassUtils.h"
#include "Obj/Gdt/GdtEntry.h"

class InfoString
{
    static const std::string EMPTY_VALUE;
    std::unordered_map<std::string, std::string> m_values;
    std::vector<std::string> m_keys_by_insertion;

public:
    _NODISCARD bool HasKey(const std::string& key) const;
    _NODISCARD const std::string& GetValueForKey(const std::string& key) const;
    const std::string& GetValueForKey(const std::string& key, bool* foundValue) const;
    void SetValueForKey(const std::string& key, std::string value);
    void RemoveKey(const std::string& key);

    _NODISCARD std::string ToString() const;
    _NODISCARD std::string ToString(const std::string& prefix) const;
    void ToGdtProperties(const std::string& prefix, GdtEntry& gdtEntry) const;

    bool FromStream(std::istream& stream);
    bool FromStream(const std::string& prefix, std::istream& stream);
};