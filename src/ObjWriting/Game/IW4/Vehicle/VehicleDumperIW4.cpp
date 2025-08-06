#include "VehicleDumperIW4.h"

#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/InfoString/EnumStrings.h"
#include "Game/IW4/InfoString/InfoStringFromStructConverter.h"
#include "Game/IW4/ObjConstantsIW4.h"
#include "Game/IW4/Vehicle/VehicleFields.h"
#include "Vehicle/VehicleCommon.h"

#include <cassert>
#include <sstream>
#include <type_traits>

using namespace IW4;

namespace
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

            case VFT_AXLE_STEERING:
            case VFT_AXLE_POWER:
            case VFT_AXLE_BRAKING:
                FillFromEnumInt(std::string(field.szName), field.iOffset, s_vehicleAxleTypeNames, std::extent_v<decltype(s_vehicleAxleTypeNames)>);
                break;

            case VFT_TROPHY_TAGS:
            {
                const auto* trophyTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                std::stringstream ss;
                bool first = true;

                for (auto i = 0u; i < std::extent_v<decltype(VehicleDef::trophyTags)>; i++)
                {
                    const auto& str = m_get_scr_string(trophyTags[i]);
                    if (!str.empty())
                    {
                        if (!first)
                            ss << "\n";
                        else
                            first = false;

                        ss << str;
                    }
                }

                m_info_string.SetValueForKey(std::string(field.szName), ss.str());
                break;
            }

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

    InfoString CreateInfoString(XAssetInfo<VehicleDef>* asset)
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
} // namespace

namespace vehicle
{
    bool DumperIW4::ShouldDump(XAssetInfo<VehicleDef>* asset)
    {
        return true;
    }

    void DumperIW4::DumpAsset(AssetDumpingContext& context, XAssetInfo<VehicleDef>* asset)
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
            const auto assetFile = context.OpenAssetFile(GetFileNameForAssetName(asset->m_name));

            if (!assetFile)
                return;

            auto& stream = *assetFile;
            const auto infoString = CreateInfoString(asset);
            const auto stringValue = infoString.ToString(ObjConstants::INFO_STRING_PREFIX_VEHICLE);
            stream.write(stringValue.c_str(), stringValue.size());
        }
    }
} // namespace vehicle
