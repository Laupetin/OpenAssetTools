#include "FxElemDefDumper.h"

#include "Utils/Logging/Log.h"

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

void FxElemDefDumper::Init(int version) const
{
    if (version == -1)
    {
        con::warn("No FxElemDef version provided, defaulting to version 1.");
        m_stream << std::format("iwfx {}\n", 1);
    }
    else
    {
        m_stream << std::format("iwfx {}\n", version);
    }
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

void FxElemDefDumper::WriteKeyValue(const std::string& key, const char* value) const
{
    assert(m_flags.m_in_element);

    std::string strVal = "";
    if (value)
    {
        strVal = value;
    }

    WriteKeyValue(key, strVal);
}

void FxElemDefDumper::WriteFloatArray(const std::string& key, const float* floatArray, unsigned int floatCount, int precision) const
{
    assert(m_flags.m_in_element);

    Indent();
    m_stream << key << " ";

    for (int i = 0; i < floatCount; i++)
    {
        std::ios oldState(nullptr);
        oldState.copyfmt(m_stream);

        if (precision > -1)
        {
            m_stream << std::fixed << std::setprecision(precision);
        }
        else if (floatArray[i] == static_cast<int>(floatArray[i]))
        {
            m_stream << std::fixed << std::setprecision(0);
        }

        m_stream << " " << *(floatArray + i);
        m_stream.copyfmt(oldState);
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
    m_stream << "spawnLooping " << fxSpawnDef.lowerBound << " " << fxSpawnDef.upperBound << ";\n";

    Indent();
    m_stream << "spawnOneShot " << fxSpawnDef.lowerBound << " " << fxSpawnDef.upperBound << ";\n";
}

void FxElemDefDumper::WriteFxFloatRange(const std::string& key, const FxFloatRange& fxFloatRange, int precision) const
{
    std::ios oldState(nullptr);
    oldState.copyfmt(m_stream);

    Indent();
    m_stream << key << " ";

    if (precision > -1)
    {
        m_stream << std::fixed << std::setprecision(precision);
    }
    else if (fxFloatRange.base == static_cast<int>(fxFloatRange.base) || fxFloatRange.amplitude == static_cast<int>(fxFloatRange.amplitude))
    {
        m_stream << std::fixed << std::setprecision(0);
    }

    m_stream << fxFloatRange.base << " " << fxFloatRange.amplitude << ";\n";
    m_stream.copyfmt(oldState);
}

void FxElemDefDumper::WriteFxIntRange(const std::string& key, const FxIntRange& fxIntRange) const
{
    Indent();
    m_stream << key << " " << fxIntRange.base << " " << fxIntRange.amplitude << ";\n";
}

void FxElemDefDumper::WriteVelGraph(const char* name,
                                    const float* localBase,
                                    const float* localAmp,
                                    const float* localDeltaBase,
                                    const float* localDeltaAmp,
                                    const float* worldBase,
                                    const float* worldAmp,
                                    const float* worldDeltaBase,
                                    const float* worldDeltaAmp,
                                    float scale,
                                    int precision)
{
    std::ios oldState(nullptr);
    oldState.copyfmt(m_stream);

    if (precision > -1)
    {
        m_stream << std::fixed << std::setprecision(precision);
    }

    if (scale == 0.0f)
    {
        scale = 1.0f;
    }

    static constexpr char AXIS_NAMES[3] = {'X', 'Y', 'Z'};

    for (int axis = 0; axis < 3; axis++)
    {
        //
        // COD graph format stores normalized values in range:
        // [-0.5, 0.5] for velocity graphs.
        //
        // Actual runtime value:
        // value = normalized * scale
        //

        const float localStart = (localBase[axis] - localAmp[axis]) / scale;

        const float localEnd = ((localBase[axis] + localDeltaBase[axis]) - (localAmp[axis] + localDeltaAmp[axis])) / scale;

        const float worldStart = (worldBase[axis] - worldAmp[axis]) / scale;

        const float worldEnd = ((worldBase[axis] + worldDeltaBase[axis]) - (worldAmp[axis] + worldDeltaAmp[axis])) / scale;

        Indent();
        m_stream << std::format("{}{}", name, AXIS_NAMES[axis]) << " " << scale << "\n";

        Indent();
        m_stream << "{\n";
        IncIndent();

        //
        // Local curve
        //
        Indent();
        m_stream << "{\n";
        IncIndent();

        Indent();
        m_stream << "0.000000 " << localStart << "\n";

        Indent();
        m_stream << "1.000000 " << localEnd << "\n";

        DecIndent();
        Indent();
        m_stream << "}\n";

        //
        // World curve
        //
        Indent();
        m_stream << "{\n";
        IncIndent();

        Indent();
        m_stream << "0.000000 " << worldStart << "\n";

        Indent();
        m_stream << "1.000000 " << worldEnd << "\n";

        DecIndent();
        Indent();
        m_stream << "}\n";

        DecIndent();
        Indent();
        m_stream << "}\n";
    }

    m_stream.copyfmt(oldState);
}

void FxElemDefDumper::WriteAlphaGraph(const char* name, const char baseAlpha, const char ampAlpha, float scale, int precision)
{
    std::ios oldState(nullptr);
    oldState.copyfmt(m_stream);

    if (precision > -1)
    {
        m_stream << std::fixed << std::setprecision(precision);
    }

    const float baseA = (unsigned char)baseAlpha / 255.0f;

    const float ampA = (unsigned char)ampAlpha / 255.0f;

    Indent();
    m_stream << name << " " << scale << "\n";

    Indent();
    m_stream << "{\n";
    IncIndent();

    //
    // Base curve
    //
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << "0.000000 " << baseA << "\n";

    Indent();
    m_stream << "1.000000 " << baseA << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    //
    // Amplitude curve
    //
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << "0.000000 " << ampA << "\n";

    Indent();
    m_stream << "1.000000 " << ampA << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    m_stream.copyfmt(oldState);
}

void FxElemDefDumper::WriteColorGraph(const char* name, const char baseColor[4], const char ampColor[4], float scale, int precision)
{
    std::ios oldState(nullptr);
    oldState.copyfmt(m_stream);

    if (precision > -1)
    {
        m_stream << std::fixed << std::setprecision(precision);
    }

    const float baseR = (unsigned char)baseColor[0] / 255.0f;
    const float baseG = (unsigned char)baseColor[1] / 255.0f;
    const float baseB = (unsigned char)baseColor[2] / 255.0f;

    const float ampR = (unsigned char)ampColor[0] / 255.0f;
    const float ampG = (unsigned char)ampColor[1] / 255.0f;
    const float ampB = (unsigned char)ampColor[2] / 255.0f;

    Indent();
    m_stream << name << " " << scale << "\n";

    Indent();
    m_stream << "{\n";
    IncIndent();

    //
    // Base curve
    //
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << "0.000000 " << baseR << " " << baseG << " " << baseB << "\n";

    Indent();
    m_stream << "1.000000 " << baseR << " " << baseG << " " << baseB << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    //
    // Amplitude curve
    //
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << "0.000000 " << ampR << " " << ampG << " " << ampB << "\n";

    Indent();
    m_stream << "1.000000 " << ampR << " " << ampG << " " << ampB << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    m_stream.copyfmt(oldState);
}

void FxElemDefDumper::WriteScaleGraph(const char* name, float baseScale, float ampScale, float scale, int precision)
{
    std::ios oldState(nullptr);
    oldState.copyfmt(m_stream);

    if (precision > -1)
    {
        m_stream << std::fixed << std::setprecision(precision);
    }

    if (scale == 0.0f)
    {
        scale = 1.0f;
    }

    //
    // Scale graphs are normalized to [0, 1]
    //

    const float start = (baseScale - ampScale) / scale;

    const float end = (baseScale + ampScale) / scale;

    Indent();
    m_stream << name << " " << scale << "\n";

    Indent();
    m_stream << "{\n";
    IncIndent();

    //
    // First curve
    //
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << "0.000000 " << start << "\n";

    Indent();
    m_stream << "1.000000 " << start << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    //
    // Second curve
    //
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << "0.000000 " << end << "\n";

    Indent();
    m_stream << "1.000000 " << end << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    m_stream.copyfmt(oldState);
}

void FxElemDefDumper::WriteSizeGraph(const char* name, const float* baseSize, const float* ampSize, float scale, int precision)
{
    std::ios oldState(nullptr);
    oldState.copyfmt(m_stream);

    if (precision > -1)
    {
        m_stream << std::fixed << std::setprecision(precision);
    }

    Indent();
    m_stream << name << " " << scale << "\n";

    Indent();
    m_stream << "{\n";
    IncIndent();

    //
    // X/base curve
    //
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << "0.000000 " << baseSize[0] << "\n";

    Indent();
    m_stream << "1.000000 " << baseSize[0] << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    //
    // Y/amplitude curve
    //
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << "0.000000 " << ampSize[0] << "\n";

    Indent();
    m_stream << "1.000000 " << ampSize[0] << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    m_stream.copyfmt(oldState);
}

void FxElemDefDumper::WriteRotGraph(const char* name, float baseRot, float ampRot, float scale, int precision)
{
    std::ios oldState(nullptr);
    oldState.copyfmt(m_stream);

    if (precision > -1)
    {
        m_stream << std::fixed << std::setprecision(precision);
    }

    Indent();
    m_stream << name << " " << scale << "\n";

    Indent();
    m_stream << "{\n";
    IncIndent();

    //
    // Base curve
    //
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << "0.000000 " << baseRot << "\n";

    Indent();
    m_stream << "1.000000 " << baseRot << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    //
    // Amplitude curve
    //
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << "0.000000 " << ampRot << "\n";

    Indent();
    m_stream << "1.000000 " << ampRot << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    DecIndent();
    Indent();
    m_stream << "}\n";

    m_stream.copyfmt(oldState);
}

void FxElemDefDumper::WriteSpawnSound(const std::string& key, const char* soundName) 
{
    std::string soundNameStr = "";
    if (soundName)
    {
        soundNameStr = soundName;
    }
    WriteSpawnSound(key, soundNameStr);
}

void FxElemDefDumper::WriteSpawnSound(const std::string& key, const std::string& soundName)
{
    Indent();
    m_stream << key << "\n";

    Indent();
    m_stream << "{\n";

    IncIndent();
    if (!soundName.empty())
    {
        Indent();
        m_stream << "\"" << soundName << "\"\n";
    }
    DecIndent();

    Indent();
    m_stream << "};\n";
}

void FxElemDefDumper::WriteFloat(const std::string& key, const float& value, int precision) const
{
    std::ios oldState(nullptr);
    oldState.copyfmt(m_stream);

    Indent();
    m_stream << key << " ";

    if (precision > -1)
    {
        m_stream << std::fixed << std::setprecision(precision);
    }
    else if (value == static_cast<int>(value))
    {
        m_stream << std::fixed << std::setprecision(0);
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
