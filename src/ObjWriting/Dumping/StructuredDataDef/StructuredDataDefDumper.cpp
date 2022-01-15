#include "StructuredDataDefDumper.h"

#include <cassert>

StructuredDataDefDumper::StructuredDataDefDumper(std::ostream& stream)
    : AbstractTextDumper(stream),
      m_flags{}
{
}

void StructuredDataDefDumper::BeginVersion(const int version)
{
    assert(!m_flags.m_in_version);
    if (m_flags.m_in_version)
        return;

    if (m_flags.m_empty_line_before_version)
        m_stream << "\n";
    else
        m_flags.m_empty_line_before_version = true;

    Indent();
    m_stream << "version " << version << "\n";

    Indent();
    m_stream << "{\n";

    IncIndent();

    m_flags.m_in_version = true;
}

void StructuredDataDefDumper::EndVersion()
{
    assert(m_flags.m_in_version);
    if (!m_flags.m_in_version)
        return;

    DecIndent();
    Indent();
    m_stream << "}\n";
    m_flags.m_in_version = false;
}
