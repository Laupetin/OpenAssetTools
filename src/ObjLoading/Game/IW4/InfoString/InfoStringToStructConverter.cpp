#include "InfoStringToStructConverter.h"

#include "Utils/Logging/Log.h"

#include <cassert>
#include <format>
#include <iostream>

using namespace IW4;

InfoStringToStructConverter::InfoStringToStructConverter(const InfoString& infoString,
                                                         void* structure,
                                                         ZoneScriptStrings& zoneScriptStrings,
                                                         MemoryManager& memory,
                                                         AssetCreationContext& context,
                                                         GenericAssetRegistration& registration,
                                                         const cspField_t* fields,
                                                         const size_t fieldCount)
    : InfoStringToStructConverterBase(infoString, structure, zoneScriptStrings, memory, context, registration),
      m_fields(fields),
      m_field_count(fieldCount)
{
}

bool InfoStringToStructConverter::ConvertBaseField(const cspField_t& field, const std::string& value)
{
    switch (static_cast<csParseFieldType_t>(field.iFieldType))
    {
    case CSPFT_STRING:
        return ConvertString(value, field.iOffset);

    case CSPFT_STRING_MAX_STRING_CHARS:
        return ConvertStringBuffer(value, field.iOffset, 1024);

    case CSPFT_STRING_MAX_QPATH:
        return ConvertStringBuffer(value, field.iOffset, 64);

    case CSPFT_STRING_MAX_OSPATH:
        return ConvertStringBuffer(value, field.iOffset, 256);

    case CSPFT_INT:
        return ConvertInt(value, field.iOffset);

    case CSPFT_BOOL:
        return ConvertBool(value, field.iOffset);

    case CSPFT_QBOOLEAN:
        return ConvertQBoolean(value, field.iOffset);

    case CSPFT_FLOAT:
        return ConvertFloat(value, field.iOffset);

    case CSPFT_MILLISECONDS:
        return ConvertMilliseconds(value, field.iOffset);

    case CSPFT_FX:
    {
        if (value.empty())
        {
            *reinterpret_cast<FxEffectDef**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
            return true;
        }

        auto* fx = m_context.LoadDependency<AssetFx>(value);

        if (fx == nullptr)
        {
            con::error("Failed to load fx asset \"{}\"", value);
            return false;
        }

        m_registration.AddDependency(fx);
        *reinterpret_cast<FxEffectDef**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = fx->Asset();

        return true;
    }

    case CSPFT_XMODEL:
    {
        if (value.empty())
        {
            *reinterpret_cast<XModel**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
            return true;
        }

        auto* xmodel = m_context.LoadDependency<AssetXModel>(value);

        if (xmodel == nullptr)
        {
            con::error("Failed to load xmodel asset \"{}\"", value);
            return false;
        }

        m_registration.AddDependency(xmodel);
        *reinterpret_cast<XModel**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = xmodel->Asset();

        return true;
    }

    case CSPFT_MATERIAL:
    {
        if (value.empty())
        {
            *reinterpret_cast<Material**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
            return true;
        }

        auto* material = m_context.LoadDependency<AssetMaterial>(value);

        if (material == nullptr)
        {
            con::error("Failed to load material asset \"{}\"", value);
            return false;
        }

        m_registration.AddDependency(material);
        *reinterpret_cast<Material**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = material->Asset();

        return true;
    }

    case CSPFT_TRACER:
    {
        if (value.empty())
        {
            *reinterpret_cast<TracerDef**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
            return true;
        }

        auto* tracer = m_context.LoadDependency<AssetTracer>(value);

        if (tracer == nullptr)
        {
            con::error("Failed to load tracer asset \"{}\"", value);
            return false;
        }

        m_registration.AddDependency(tracer);
        *reinterpret_cast<TracerDef**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = tracer->Asset();

        return true;
    }

    case CSPFT_MPH_TO_INCHES_PER_SEC:
    {
        char* endPtr;
        *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = strtof(value.c_str(), &endPtr) * 17.6f;

        if (endPtr != &value[value.size()])
        {
            con::error("Failed to parse value \"{}\" as mph", value);
            return false;
        }

        return true;
    }

    case CSPFT_PHYS_COLLMAP:
    {
        if (value.empty())
        {
            *reinterpret_cast<PhysCollmap**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
            return true;
        }

        auto* collmap = m_context.LoadDependency<AssetPhysCollMap>(value);

        if (collmap == nullptr)
        {
            con::error("Failed to load collmap asset \"{}\"", value);
            return false;
        }

        m_registration.AddDependency(collmap);
        *reinterpret_cast<PhysCollmap**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = collmap->Asset();

        return true;
    }

    case CSPFT_SOUND:
    {
        if (value.empty())
        {
            reinterpret_cast<SndAliasCustom*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset)->name = nullptr;
            return true;
        }

        auto* name = m_memory.Alloc<snd_alias_list_name>();
        name->soundName = m_memory.Dup(value.c_str());

        reinterpret_cast<SndAliasCustom*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset)->name = name;

        m_registration.AddIndirectAssetReference(m_context.LoadIndirectAssetReference<AssetSound>(value));
        return true;
    }

    case CSPFT_NUM_BASE_FIELD_TYPES:
    default:
        assert(false);
        return false;
    }
}

bool InfoStringToStructConverter::Convert()
{
    for (auto fieldIndex = 0u; fieldIndex < m_field_count; fieldIndex++)
    {
        const auto& field = m_fields[fieldIndex];
        assert(field.iFieldType >= 0);

        auto foundValue = false;
        const auto& value = m_info_string.GetValueForKey(std::string(field.szName), &foundValue);

        if (foundValue)
        {
            if (field.iFieldType < CSPFT_NUM_BASE_FIELD_TYPES)
            {
                if (!ConvertBaseField(field, value))
                    return false;
            }
            else
            {
                if (!ConvertExtensionField(field, value))
                    return false;
            }
        }
    }

    return true;
}
