#include "FxElemDefDumper.h"

#include <cassert>
#include <iomanip>

using namespace fx_elem_def;

FxElemDefDumper::FxElemDefDumper(std::ostream& stream)
    : AbstractTextDumper(stream),
      m_flags{},
      m_element_index(0u)
{
    UseTabForIndent(true);
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

void FxElemDefDumper::BeginVisuals(const std::string& key)
{
    assert(!m_flags.m_in_visuals && m_flags.m_in_element);
    if (!m_flags.m_in_element || m_flags.m_in_visuals)
        return;

    Indent();
    m_stream << key << "\n";

    Indent();
    m_stream << "{\n";

    IncIndent();
    m_flags.m_in_visuals = true;
}

void FxElemDefDumper::EndVisuals()
{
    assert(m_flags.m_in_visuals);
    if (!m_flags.m_in_visuals)
        return;

    DecIndent();
    Indent();
    m_stream << "};\n";
    m_flags.m_in_visuals = false;
}

void FxElemDefDumper::WriteVisualEntry(const std::string& entry) const
{
    assert(m_flags.m_in_visuals);

    Indent();
    m_stream << "\"" << entry << "\"\n";
}

void FxElemDefDumper::BeginEnum(const std::string& key)
{
    assert(!m_flags.m_in_enum && m_flags.m_in_element);
    if (!m_flags.m_in_element || m_flags.m_in_enum)
        return;

    Indent();
    m_stream << key;

    m_flags.m_in_enum = true;
}

void FxElemDefDumper::EndEnum()
{
    assert(m_flags.m_in_enum);
    if (!m_flags.m_in_enum)
        return;

    m_stream << ";\n";
    m_flags.m_in_enum = false;
}

void FxElemDefDumper::WriteEnumEntry(const std::string& entry) const
{
    assert(m_flags.m_in_enum);

    m_stream << " " << entry;
}

void FxElemDefDumper::WriteKeyValue(const std::string& key, const std::string& value) const
{
    assert(m_flags.m_in_element);

    Indent();
    m_stream << "" << key << " \"" << value << "\";\n";
}

void FxElemDefDumper::WriteFloatArray(const std::string& key, const float* floatArray, unsigned int floatCount) const
{
    assert(m_flags.m_in_element);

    Indent();
    m_stream << key;
    for (unsigned int i = 0; i < floatCount; i++)
    {
        m_stream << " " << *(floatArray + i);
    }
    m_stream << ";\n";
}

void FxElemDefDumper::WriteEnum(const std::string& key, unsigned char val, const char* enumStrings[]) const
{
    assert(m_flags.m_in_element);

    Indent();
    m_stream << "" << key << " \"" << enumStrings[val] << "\";\n";
}

void FxElemDefDumper::WriteFxElemDefVisuals(const FxElemDefVisuals& fxElemDefVisuals,
                                            unsigned char visualCount,
                                            FxElemType elemType,
                                            const char* elemTypeStrings[])
{
    assert(m_flags.m_in_element);

    Indent();
    m_stream << elemTypeStrings[elemType] << "\r\n";

    Indent();
    m_stream << "{";

    IncIndent();
    Indent();

    if (elemType == FX_ELEM_TYPE_DECAL)
    {
        // Load FxElemMarkVisualsArray
    }
    else if (visualCount < 1u)
    {
        // Load FxElemVisualsArray
    }
    else if (elemType == FX_ELEM_TYPE_MODEL)
    {
        // Load xmodel
    }
    else if (elemType == FX_ELEM_TYPE_SOUND)
    {
        // Load sound name
    }
    else if (elemType == FX_ELEM_TYPE_RUNNER)
    {
        // Load FxEffectDef + name?
    }
    else if (elemType != FX_ELEM_TYPE_OMNI_LIGHT && elemType != FX_ELEM_TYPE_SPOT_LIGHT)
    {
        // Load material
    }

    DecIndent();
    Indent();
    m_stream << "};";
}

void FxElemDefDumper::WriteFxSpawnDef(const FxSpawnDef& fxSpawnDef) const
{
    Indent();
    m_stream << "spawnLooping " << fxSpawnDef.lowerBound * 100 << " " << fxSpawnDef.upperBound * 2 << ";\n";

    Indent();
    m_stream << "spawnOneShot " << fxSpawnDef.lowerBound << " " << fxSpawnDef.upperBound << ";\n";
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

void FxElemDefDumper::WriteFloat(const std::string& key, const float& value, int precision) const
{
    std::ios oldState(nullptr);
    oldState.copyfmt(m_stream);

    Indent();
    m_stream << key << " ";

    if (value == static_cast<int>(value))
    {
        m_stream << std::fixed << std::setprecision(0);
    }
    else if (precision != -1)
    {
        m_stream << std::fixed << std::setprecision(precision);
    }

    m_stream << value << ";\n";
    m_stream.copyfmt(oldState);
}

void FxElemDefDumper::WriteInt(const std::string& key, const int& value) const
{
    Indent();
    m_stream << key << " " << value << ";\n";
}

void FxElemDefDumper::WriteBinary(const std::string& key, const int& value) const
{
    Indent();

    m_stream << key << " ";
    for (int i = 0; i < 32; i++)
    {
        if (value & (1 << i))
        {
            m_stream << i << " ";
        }
    }
    m_stream << " - ";
    for (int i = 0; i < 32; i++)
    {
        if (value & (1 << i))
        {
            m_stream << 1;
        }
        else
        {
            m_stream << 0;
        }
    }
    m_stream << "\n";
}
