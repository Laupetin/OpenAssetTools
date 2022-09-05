#include "DirectiveEscapeStreamProxy.h"

using namespace templating;

DirectiveEscapeStreamProxy::DirectiveEscapeStreamProxy(IParserLineStream* stream)
    : m_stream(stream)
{
}

ParserLine DirectiveEscapeStreamProxy::NextLine()
{
    auto line = m_stream->NextLine();

    if (line.m_line.size() >= 2 && line.m_line[0] == '#' && line.m_line[1] == '#')
        line.m_line = line.m_line.substr(1);

    return line;
}

bool DirectiveEscapeStreamProxy::IncludeFile(const std::string& filename)
{
    return m_stream->IncludeFile(filename);
}

void DirectiveEscapeStreamProxy::PopCurrentFile()
{
    m_stream->PopCurrentFile();
}

bool DirectiveEscapeStreamProxy::IsOpen() const
{
    return m_stream->IsOpen();
}

bool DirectiveEscapeStreamProxy::Eof() const
{
    return m_stream->Eof();
}
