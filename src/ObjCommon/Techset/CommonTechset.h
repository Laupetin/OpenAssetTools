#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace techset
{
    class CommonTechniqueTypeNames
    {
    public:
        CommonTechniqueTypeNames(const char** names, size_t nameCount);

        [[nodiscard]] const char* GetTechniqueTypeName(size_t techniqueTypeIndex) const;
        [[nodiscard]] std::optional<size_t> GetTechniqueTypeByName(const std::string& name) const;
        [[nodiscard]] size_t GetTechniqueTypeCount() const;

    private:
        std::vector<const char*> m_names;
        std::unordered_map<std::string, size_t> m_technique_type_lookup;
    };

    class CommonTechset
    {
    public:
        CommonTechset() = default;
        explicit CommonTechset(size_t techniqueTypeCount);
        CommonTechset(std::string name, std::vector<std::string> techniqueNames);

        std::string m_name;
        std::vector<std::string> m_technique_names;
    };
} // namespace techset
