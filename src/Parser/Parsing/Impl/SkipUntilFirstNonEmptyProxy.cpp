#include "SkipUntilFirstNonEmptyProxy.h"

SkipUntilFirstNonEmptyProxy::SkipUntilFirstNonEmptyProxy(IParserLineStream* stream)
    : m_stream(stream),
      m_had_non_empty(false)
{
}

ParserLine SkipUntilFirstNonEmptyProxy::NextLine()
{
    auto line = m_stream->NextLine();
    if (!m_had_non_empty)
    {
        while (line.m_line.empty() && !m_stream->Eof())
            line = m_stream->NextLine();
        m_had_non_empty = true;
    }

    return line;
}

bool SkipUntilFirstNonEmptyProxy::IncludeFile(const std::string& filename)
{
    return m_stream->IncludeFile(filename);
}

void SkipUntilFirstNonEmptyProxy::PopCurrentFile()
{
    m_stream->PopCurrentFile();
}

bool SkipUntilFirstNonEmptyProxy::IsOpen() const
{
    return m_stream->IsOpen();
}

bool SkipUntilFirstNonEmptyProxy::Eof() const
{
    return m_stream->Eof();
}
