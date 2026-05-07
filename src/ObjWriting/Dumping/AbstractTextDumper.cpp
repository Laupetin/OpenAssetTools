#include "AbstractTextDumper.h"

#include <cassert>

AbstractTextDumper::AbstractTextDumper(std::ostream& stream)
    : m_stream(stream),
      m_indent(0u),
      m_tab_indent(false)
{
}

void AbstractTextDumper::UseTabForIndent(bool tab_indent) 
{
    m_tab_indent = tab_indent;
}

void AbstractTextDumper::Indent() const
{
    for (auto i = 0u; i < m_indent; i++)
    {
        if (m_tab_indent)
        {
            m_stream << "\t";
        }
        else
        {
            m_stream << "  ";
        }
    }
}

void AbstractTextDumper::IncIndent()
{
    ++m_indent;
}

void AbstractTextDumper::DecIndent()
{
    assert(m_indent > 0);
    if (m_indent > 0)
        m_indent--;
}
