#pragma once

#include <ostream>

class AbstractTextDumper
{
protected:
    std::ostream& m_stream;
    size_t m_indent;
    bool m_tab_indent;

    void UseTabForIndent(bool tab_indent);
    void Indent() const;
    void IncIndent();
    void DecIndent();

    explicit AbstractTextDumper(std::ostream& stream);
};
