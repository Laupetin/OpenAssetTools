#pragma once

#include "Obj/Gdt/GdtEntry.h"
#include "Utils/StreamUtils.h"
#include "Utils/StringUtils.h"

#include <istream>
#include <string>
#include <unordered_map>
#include <vector>

class InfoString
{
public:
    InfoString() = default;

    [[nodiscard]] bool HasKey(const std::string& key) const;
    [[nodiscard]] const std::string& GetValueForKey(const std::string& key) const;
    const std::string& GetValueForKey(const std::string& key, bool* foundValue) const;
    void SetValueForKey(const std::string& key, std::string value);
    void RemoveKey(const std::string& key);

    [[nodiscard]] std::string ToString() const;
    [[nodiscard]] std::string ToString(const std::string& prefix) const;
    void ToGdtProperties(const std::string& prefix, GdtEntry& gdtEntry) const;

    bool FromStream(std::istream& stream);
    bool FromStream(const std::string& prefix, std::istream& stream);
    bool FromGdtProperties(const GdtEntry& gdtEntry);

private:
    class HashValue
    {
    public:
        std::size_t operator()(const std::string& s) const noexcept
        {
            std::string lower(s);
            utils::MakeStringLowerCase(lower);
            return std::hash<std::string>{}(lower);
        }
    };

    class EqualValue
    {
    public:
        constexpr bool operator()(const std::string& lhs, const std::string& rhs) const
        {
            return utils::StringEqualsIgnoreCase(lhs, rhs);
        }
    };

    std::unordered_map<std::string, std::string, HashValue, EqualValue> m_value_lookup;
    std::vector<std::string> m_keys_by_insertion;
};
