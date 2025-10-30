#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace techset
{
    class CommonTechniqueTypeNames
    {
    public:
        CommonTechniqueTypeNames(const char** names, size_t nameCount);

        [[nodiscard]] const char* GetTechniqueTypeName(size_t techniqueTypeIndex) const;
        [[nodiscard]] size_t GetTechniqueTypeCount() const;

    private:
        std::vector<const char*> m_names;
    };

    class CommonTechset
    {
    public:
        std::string m_name;
        std::vector<std::string> m_technique_names;
    };
} // namespace techset
