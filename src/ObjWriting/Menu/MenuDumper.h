#pragma once

#include <string>
#include <ostream>

class MenuDumper
{
protected:
    std::ostream& m_stream;
    size_t m_indent;

    void IncIndent();
    void DecIndent();
    void Indent() const;

public:
    explicit MenuDumper(std::ostream& stream);

    void Start();
    void End();

    void IncludeMenu(const std::string& menuPath) const;
};