#include "InfoStringToStructConverter.h"

#include <cassert>
#include <iostream>

#include "Game/T6/CommonT6.h"

using namespace T6;

InfoStringToStructConverter::InfoStringToStructConverter(const InfoString& infoString, void* structure, ZoneScriptStrings& zoneScriptStrings, MemoryManager* memory, IAssetLoadingManager* manager,
                                                         const cspField_t* fields, const size_t fieldCount)
    : InfoStringToStructConverterBase(infoString, structure, zoneScriptStrings, memory),
      m_loading_manager(manager),
      m_fields(fields),
      m_field_count(fieldCount)
{
}

bool InfoStringToStructConverter::GetHashValue(const std::string& value, unsigned& hash)
{
    if (!value.empty() && value[0] == '@')
    {
        char* endPtr;
        hash = strtoul(&value[1], &endPtr, 16);
        return endPtr == &value[value.size()];
    }

    hash = CommonT6::Com_HashString(value.c_str());
    return true;
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

    case CSPFT_UINT:
        return ConvertUint(value, field.iOffset);

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
                *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
                return true;
            }

            auto* fx = m_loading_manager->LoadDependency(ASSET_TYPE_FX, value);

            if (fx == nullptr)
            {
                std::cout << "Failed to load fx asset \"" << value << "\"" << std::endl;
                return false;
            }

            m_dependencies.emplace(fx);
            *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = fx->m_ptr;

            return true;
        }

    case CSPFT_XMODEL:
        {
            if (value.empty())
            {
                *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
                return true;
            }

            auto* xmodel = m_loading_manager->LoadDependency(ASSET_TYPE_XMODEL, value);

            if (xmodel == nullptr)
            {
                std::cout << "Failed to load xmodel asset \"" << value << "\"" << std::endl;
                return false;
            }

            m_dependencies.emplace(xmodel);
            *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = xmodel->m_ptr;

            return true;
        }

    case CSPFT_MATERIAL:
    case CSPFT_MATERIAL_STREAM:
        {
            if (value.empty())
            {
                *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
                return true;
            }

            auto* material = m_loading_manager->LoadDependency(ASSET_TYPE_MATERIAL, value);

            if (material == nullptr)
            {
                std::cout << "Failed to load material asset \"" << value << "\"" << std::endl;
                return false;
            }

            m_dependencies.emplace(material);
            *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = material->m_ptr;

            return true;
        }

    case CSPFT_PHYS_PRESET:
        {
            if (value.empty())
            {
                *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
                return true;
            }

            auto* physPreset = m_loading_manager->LoadDependency(ASSET_TYPE_PHYSPRESET, value);

            if (physPreset == nullptr)
            {
                std::cout << "Failed to load physpreset asset \"" << value << "\"" << std::endl;
                return false;
            }

            m_dependencies.emplace(physPreset);
            *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = physPreset->m_ptr;

            return true;
        }

    case CSPFT_SCRIPT_STRING:
        return ConvertScriptString(value, field.iOffset);

    case CSPFT_TRACER:
        {
            if (value.empty())
            {
                *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
                return true;
            }

            auto* tracer = m_loading_manager->LoadDependency(ASSET_TYPE_TRACER, value);

            if (tracer == nullptr)
            {
                std::cout << "Failed to load tracer asset \"" << value << "\"" << std::endl;
                return false;
            }

            m_dependencies.emplace(tracer);
            *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = tracer->m_ptr;

            return true;
        }

    case CSPFT_SOUND_ALIAS_ID:
        {
            unsigned int soundAliasHash;
            if (!GetHashValue(value, soundAliasHash))
            {
                std::cout << "Failed to parse value \"" << value << "\" as hash" << std::endl;
                return false;
            }

            *reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = soundAliasHash;
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
