#include "InfoStringFromStructConverter.h"

#include <cassert>
#include <sstream>

using namespace T6;

void InfoStringFromStructConverter::FillFromBaseField(const cspField_t& field)
{
    switch (static_cast<csParseFieldType_t>(field.iFieldType))
    {
    case CSPFT_STRING:
        FillFromString(std::string(field.szName), field.iOffset);
        break;

    case CSPFT_STRING_MAX_STRING_CHARS:
        FillFromStringBuffer(std::string(field.szName), field.iOffset, 1024);
        break;

    case CSPFT_STRING_MAX_QPATH:
        FillFromStringBuffer(std::string(field.szName), field.iOffset, 64);
        break;

    case CSPFT_STRING_MAX_OSPATH:
        FillFromStringBuffer(std::string(field.szName), field.iOffset, 256);
        break;

    case CSPFT_INT:
        FillFromInt(std::string(field.szName), field.iOffset);
        break;

    case CSPFT_UINT:
        FillFromUint(std::string(field.szName), field.iOffset);
        break;

    case CSPFT_BOOL:
        FillFromBool(std::string(field.szName), field.iOffset);
        break;

    case CSPFT_QBOOLEAN:
        FillFromQBoolean(std::string(field.szName), field.iOffset);
        break;

    case CSPFT_FLOAT:
        FillFromFloat(std::string(field.szName), field.iOffset);
        break;

    case CSPFT_MILLISECONDS:
        FillFromMilliseconds(std::string(field.szName), field.iOffset);
        break;

    case CSPFT_FX:
    {
        const auto* fx = *reinterpret_cast<FxEffectDef**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

        if (fx)
            m_info_string.SetValueForKey(std::string(field.szName), std::string(AssetName(fx->name)));
        else
            m_info_string.SetValueForKey(std::string(field.szName), "");
        break;
    }

    case CSPFT_XMODEL:
    {
        const auto* model = *reinterpret_cast<XModel**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

        if (model)
            m_info_string.SetValueForKey(std::string(field.szName), std::string(AssetName(model->name)));
        else
            m_info_string.SetValueForKey(std::string(field.szName), "");
        break;
    }

    case CSPFT_MATERIAL:
    case CSPFT_MATERIAL_STREAM:
    {
        const auto* material = *reinterpret_cast<Material**>(reinterpret_cast<uintptr_t>(m_structure) + field.
            iOffset);

        if (material)
            m_info_string.SetValueForKey(std::string(field.szName), std::string(AssetName(material->info.name)));
        else
            m_info_string.SetValueForKey(std::string(field.szName), "");
        break;
    }

    case CSPFT_PHYS_PRESET:
    {
        const auto* physPreset = *reinterpret_cast<PhysPreset**>(reinterpret_cast<uintptr_t>(m_structure) + field.
            iOffset);

        if (physPreset)
            m_info_string.SetValueForKey(std::string(field.szName), std::string(AssetName(physPreset->name)));
        else
            m_info_string.SetValueForKey(std::string(field.szName), "");
        break;
    }

    case CSPFT_SCRIPT_STRING:
        FillFromScriptString(std::string(field.szName), field.iOffset);
        break;

    case CSPFT_TRACER:
    {
        const auto* tracer = *reinterpret_cast<TracerDef**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

        if (tracer)
            m_info_string.SetValueForKey(std::string(field.szName), std::string(AssetName(tracer->name)));
        else
            m_info_string.SetValueForKey(std::string(field.szName), "");
        break;
    }

    case CSPFT_SOUND_ALIAS_ID:
    {
        // TODO: Search sound files for files matching the hash
        const auto* hash = reinterpret_cast<unsigned*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
        std::ostringstream str;
        str << '@' << std::hex << *hash;
        m_info_string.SetValueForKey(std::string(field.szName), str.str());
        break;
    }

    case CSPFT_NUM_BASE_FIELD_TYPES:
    default:
        assert(false);
        break;
    }
}

void InfoStringFromStructConverter::FillInfoString()
{
    for (auto fieldIndex = 0u; fieldIndex < m_field_count; fieldIndex++)
    {
        const auto& field = m_fields[fieldIndex];
        assert(field.iFieldType >= 0);

        if (field.iFieldType < CSPFT_NUM_BASE_FIELD_TYPES)
            FillFromBaseField(field);
        else
            FillFromExtensionField(field);
    }
}

InfoStringFromStructConverter::InfoStringFromStructConverter(const void* structure, const cspField_t* fields,
    const size_t fieldCount)
    : InfoStringFromStructConverterBase(structure),
    m_fields(fields),
    m_field_count(fieldCount)
{
}

InfoStringFromStructConverter::InfoStringFromStructConverter(const void* structure, const cspField_t* fields, const size_t fieldCount,
    std::function<std::string(scr_string_t)> scriptStringValueCallback)
    : InfoStringFromStructConverterBase(structure, std::move(scriptStringValueCallback)),
    m_fields(fields),
    m_field_count(fieldCount)
{
}
