#include "MenuDumper.h"

MenuDumper::MenuDumper(std::ostream& stream)
    : m_stream(stream),
      m_indent(0u)
{
}

void MenuDumper::IncIndent()
{
    m_indent++;
}

void MenuDumper::DecIndent()
{
    if (m_indent > 0)
        m_indent--;
}

void MenuDumper::Indent() const
{
    for (auto i = 0u; i < m_indent; i++)
        m_stream << "    ";
}

void MenuDumper::Start()
{
    Indent();
    m_stream << "{\n";
    IncIndent();
}

void MenuDumper::End()
{
    for (auto i = 0u; i < m_indent; i++)
    {
        DecIndent();
        Indent();
        m_stream << "}\n";
    }
}

void MenuDumper::IncludeMenu(const std::string& menuPath) const
{
    Indent();
    m_stream << "loadMenu { \"" << menuPath << "\" }\n";
}
