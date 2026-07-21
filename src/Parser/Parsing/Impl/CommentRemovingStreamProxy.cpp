#include "CommentRemovingStreamProxy.h"

#include "Parsing/ParsingException.h"

CommentRemovingStreamProxy::CommentRemovingStreamProxy(IParserLineStream* stream)
    : m_stream(stream),
      m_inside_multi_line_comment(false),
      m_next_line_is_comment(false),
      m_multi_line_comment_start_pos(0u)
{
}

ParserLine CommentRemovingStreamProxy::NextLine()
{
    auto line = m_stream->NextLine();

    if (line.IsEof() && m_inside_multi_line_comment)
    {
        throw ParsingException(
            TokenPos(*m_multi_line_comment_start_line.m_filename, m_multi_line_comment_start_line.m_line_number, m_multi_line_comment_start_pos + 1u),
            "Unclosed multi-line comment");
    }

    if (m_next_line_is_comment)
    {
        m_next_line_is_comment = !line.m_line.empty() && line.m_line[line.m_line.size() - 1] == '\\';
        return ParserLine(line.m_filename, line.m_line_number, std::string());
    }

    auto multiLineCommentStart = 0u;
    auto inString = false;
    auto isEscaped = false;
    for (auto i = 0u; i < line.m_line.size(); i++)
    {
        const auto c = line.m_line[i];

        if (m_inside_multi_line_comment)
        {
            if (c == '*' && i + 1 < line.m_line.size() && line.m_line[i + 1] == '/')
            {
                line.m_line.erase(multiLineCommentStart, i + 2 - multiLineCommentStart);
                i = multiLineCommentStart - 1;
                multiLineCommentStart = 0;
                m_inside_multi_line_comment = false;
            }
        }
        else if (inString)
        {
            if (isEscaped)
                isEscaped = false;
            else if (c == '\\')
                isEscaped = true;
            else if (c == '"')
                inString = false;
        }
        else
        {
            if (c == '"')
            {
                inString = true;
            }
            else if (c == '/' && i + 1 < line.m_line.size())
            {
                const auto c1 = line.m_line[i + 1];

                if (c1 == '*')
                {
                    multiLineCommentStart = i;
                    m_inside_multi_line_comment = true;
                    m_multi_line_comment_start_line = line;
                    m_multi_line_comment_start_pos = i;
                }
                else if (c1 == '/')
                {
                    m_next_line_is_comment = line.m_line[line.m_line.size() - 1] == '\\';
                    line.m_line.erase(i);
                    return line;
                }
            }
        }
    }

    if (m_inside_multi_line_comment)
        line.m_line.erase(multiLineCommentStart);

    return line;
}

bool CommentRemovingStreamProxy::IncludeFile(const std::string& filename)
{
    return m_stream->IncludeFile(filename);
}

void CommentRemovingStreamProxy::PopCurrentFile()
{
    m_stream->PopCurrentFile();
}

bool CommentRemovingStreamProxy::IsOpen() const
{
    return m_stream->IsOpen();
}

bool CommentRemovingStreamProxy::Eof() const
{
    return m_stream->Eof();
}
