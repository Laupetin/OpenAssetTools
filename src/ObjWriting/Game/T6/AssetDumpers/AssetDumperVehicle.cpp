#include "AssetDumperVehicle.h"

#include "Game/T6/InfoString/InfoStringFromStructConverter.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/Vehicle/VehicleFields.h"

#include <cassert>
#include <type_traits>

using namespace T6;

namespace T6
{
    class InfoStringFromVehicleConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<VehicleFieldType>(field.iFieldType))
            {
            case VFT_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, s_vehicleClassNames, std::extent_v<decltype(s_vehicleClassNames)>);
                break;

            case VFT_CAMERAMODE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, s_vehicleCameraModes, std::extent_v<decltype(s_vehicleCameraModes)>);
                break;

            case VFT_TRACTION_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, s_tractionTypeNames, std::extent_v<decltype(s_tractionTypeNames)>);
                break;

            case VFT_MPH_TO_INCHES_PER_SECOND:
            {
                const auto* num = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                m_info_string.SetValueForKey(std::string(field.szName), std::to_string(*num / 17.6f));
                break;
            }

            case VFT_POUNDS_TO_GAME_MASS:
            {
                const auto* num = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                m_info_string.SetValueForKey(std::string(field.szName), std::to_string(*num / 0.001f));
                break;
            }

            case VFT_TEAM:
            {
                const auto* num = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                switch (*num)
                {
                case TEAM_AXIS:
                    m_info_string.SetValueForKey(std::string(field.szName), "axis");
                    break;

                case TEAM_ALLIES:
                    m_info_string.SetValueForKey(std::string(field.szName), "allies");
                    break;

                case TEAM_FOUR:
                    m_info_string.SetValueForKey(std::string(field.szName), "neutral");
                    break;

                default:
                    assert(false);
                    m_info_string.SetValueForKey(std::string(field.szName), "");
                    break;
                }
                break;
            }

            case VFT_KEY_BINDING:
            case VFT_GRAPH:
            case VFT_WIIUCONTROLOVERRIDE:
            case VFT_NUM:
            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromVehicleConverter(const VehicleDef* structure,
                                       const cspField_t* fields,
                                       const size_t fieldCount,
                                       std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
} // namespace T6

InfoString AssetDumperVehicle::CreateInfoString(XAssetInfo<VehicleDef>* asset)
{
    InfoStringFromVehicleConverter converter(asset->Asset(),
                                             vehicle_fields,
                                             std::extent_v<decltype(vehicle_fields)>,
                                             [asset](const scr_string_t scrStr) -> std::string
                                             {
                                                 assert(scrStr < asset->m_zone->m_script_strings.Count());
                                                 if (scrStr >= asset->m_zone->m_script_strings.Count())
                                                     return "";

                                                 return asset->m_zone->m_script_strings[scrStr];
                                             });

    return converter.Convert();
}

bool AssetDumperVehicle::ShouldDump(XAssetInfo<VehicleDef>* asset)
{
    return true;
}

void AssetDumperVehicle::DumpAsset(AssetDumpingContext& context, XAssetInfo<VehicleDef>* asset)
{
    // Only dump raw when no gdt available
    if (context.m_gdt)
    {
        const auto infoString = CreateInfoString(asset);
        GdtEntry gdtEntry(asset->m_name, ObjConstants::GDF_FILENAME_VEHICLE);
        infoString.ToGdtProperties(ObjConstants::INFO_STRING_PREFIX_VEHICLE, gdtEntry);
        context.m_gdt->WriteEntry(gdtEntry);
    }
    else
    {
        const auto assetFile = context.OpenAssetFile("vehicles/" + asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        const auto infoString = CreateInfoString(asset);
        const auto stringValue = infoString.ToString(ObjConstants::INFO_STRING_PREFIX_VEHICLE);
        stream.write(stringValue.c_str(), stringValue.size());
    }
}
