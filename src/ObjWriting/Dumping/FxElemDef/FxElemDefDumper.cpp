#include "FxElemDefDumper.h"

#include <cassert>
#include <iomanip>

using namespace fx_elem_def;

FxElemDefDumper::FxElemDefDumper(std::ostream& stream)
    : AbstractTextDumper(stream)
    , m_flags{}
    , m_element_index(0u)
{
}

void FxElemDefDumper::Init() const
{
    m_stream << "iwfx 2\n";
}

void FxElemDefDumper::WriteComment(const std::string& key) const
{
    Indent();
    m_stream << "// " << key << "\n";
}

void FxElemDefDumper::BeginElement()
{
    assert(!m_flags.m_in_element);
    if (m_flags.m_in_element)
        return;

    Indent();
    m_stream << "// element " << m_element_index << "\n";

    Indent();
    m_stream << "{\n";

    IncIndent();
    m_element_index++;
    m_flags.m_in_element = true;
}

void FxElemDefDumper::EndElement()
{
    assert(m_flags.m_in_element);
    if (!m_flags.m_in_element)
        return;

    DecIndent();
    Indent();
    m_stream << "}\n";
    m_flags.m_in_element = false;
}

void FxElemDefDumper::WriteKeyValue(const std::string& key, const std::string& value) const
{
    assert(m_flags.m_in_element);

    Indent();
    m_stream << "\"" << key << "\" \"" << value << "\";\n";
}

void FxElemDefDumper::WriteFxSpawnDef(const FxSpawnDef& fxSpawnDef, unsigned int elemIndex, unsigned int elemDefCountLooping) const
{
    Indent();

    if (elemIndex < elemDefCountLooping)
    {
        const auto& l = fxSpawnDef.looping;

        m_stream << "spawnLooping " << l.intervalMsec << " " << l.count << ";\n";
    }
    else
    {
        const auto& o = fxSpawnDef.oneShot;

        m_stream << "spawnOneShot " << o.count.base << " " << o.count.amplitude << ";\n";
    }
}

void FxElemDefDumper::WriteFxFloatRange(const std::string& key, const FxFloatRange& fxFloatRange) const
{
    Indent();
    m_stream << key << " " << fxFloatRange.base << " " << fxFloatRange.amplitude << ";\n";
}

void FxElemDefDumper::WriteFxIntRange(const std::string& key, const FxIntRange& fxIntRange) const
{
    Indent();
    m_stream << key << " " << fxIntRange.base << " " << fxIntRange.amplitude << ";\n";
}

void FxElemDefDumper::WriteFloat(const std::string& key, const float& value) const
{
    Indent();
    m_stream << key << " " << value << ";\n";
}

void FxElemDefDumper::WriteInt(const std::string& key, const int& value) const
{
    Indent();
    m_stream << key << " " << value << ";\n";
}
