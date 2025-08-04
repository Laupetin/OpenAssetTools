#include "InfoStringLoaderVehicleT6.h"

#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/T6.h"
#include "Game/T6/Vehicle/VehicleFields.h"

#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace T6;

namespace
{
    class InfoStringToVehicleConverter final : public InfoStringToStructConverter
    {
    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<VehicleFieldType>(field.iFieldType))
            {
            case VFT_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, s_vehicleClassNames, std::extent_v<decltype(s_vehicleClassNames)>);

            case VFT_CAMERAMODE:
                return ConvertEnumInt(field.szName, value, field.iOffset, s_vehicleCameraModes, std::extent_v<decltype(s_vehicleCameraModes)>);

            case VFT_TRACTION_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, s_tractionTypeNames, std::extent_v<decltype(s_tractionTypeNames)>);

            case VFT_MPH_TO_INCHES_PER_SECOND:
            {
                char* endPtr;
                *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = strtof(value.c_str(), &endPtr) * 17.6f;

                if (endPtr != &value[value.size()])
                {
                    std::cerr << std::format("Failed to parse value \"{}\" as mph\n", value);
                    return false;
                }

                return true;
            }

            case VFT_POUNDS_TO_GAME_MASS:
            {
                char* endPtr;
                *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = strtof(value.c_str(), &endPtr) * 0.001f;

                if (endPtr != &value[value.size()])
                {
                    std::cerr << std::format("Failed to parse value \"{}\" as pounds\n", value);
                    return false;
                }

                return true;
            }

            case VFT_TEAM:
            {
                if (value == "axis")
                {
                    *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = TEAM_AXIS;
                    return true;
                }

                if (value == "allies")
                {
                    *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = TEAM_ALLIES;
                    return true;
                }

                if (value == "neutral")
                {
                    *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = TEAM_FOUR;
                    return true;
                }

                *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = TEAM_BAD;
                std::cerr << std::format("Failed to parse value \"{}\" as team\n", value);
                return false;
            }

            case VFT_KEY_BINDING:
            case VFT_GRAPH:
            case VFT_WIIUCONTROLOVERRIDE:
            default:
                assert(false);
                return false;
            }
        }

    public:
        InfoStringToVehicleConverter(const InfoString& infoString,
                                     VehicleDef& vehicleDef,
                                     ZoneScriptStrings& zoneScriptStrings,
                                     MemoryManager& memory,
                                     AssetCreationContext& context,
                                     AssetRegistration<AssetVehicle>& registration,
                                     const cspField_t* fields,
                                     const size_t fieldCount)
            : InfoStringToStructConverter(infoString, &vehicleDef, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
        }
    };
} // namespace

namespace T6::vehicle
{
    InfoStringLoader::InfoStringLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
        : m_memory(memory),
          m_search_path(searchPath),
          m_zone(zone)
    {
    }

    AssetCreationResult InfoStringLoader::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
    {
        auto* vehicleDef = m_memory.Alloc<VehicleDef>();
        vehicleDef->name = m_memory.Dup(assetName.c_str());

        AssetRegistration<AssetVehicle> registration(assetName, vehicleDef);

        InfoStringToVehicleConverter converter(
            infoString, *vehicleDef, m_zone.m_script_strings, m_memory, context, registration, vehicle_fields, std::extent_v<decltype(vehicle_fields)>);
        if (!converter.Convert())
        {
            std::cerr << std::format("Failed to parse vehicle: \"{}\"\n", assetName);
            return AssetCreationResult::Failure();
        }

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }
} // namespace T6::vehicle
