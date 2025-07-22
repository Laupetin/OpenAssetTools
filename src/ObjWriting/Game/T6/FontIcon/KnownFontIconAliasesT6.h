#pragma once

#include <string>

namespace T6
{
    class KnownAlias
    {
    public:
        explicit KnownAlias(std::string aliasName);

        std::string m_name;
        int m_hash;
    };

    const KnownAlias* FindKnownFontIconAliasByHash(int hash);
} // namespace T6
