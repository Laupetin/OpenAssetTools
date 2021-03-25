#include "AssetDumperPhysConstraints.h"

#include <cassert>
#include <type_traits>

#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringFromStructConverter.h"
#include "Game/T6/InfoString/PhysConstraintsFields.h"

using namespace T6;

namespace T6
{
    class InfoStringFromPhysConstraintsConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<constraintsFieldType_t>(field.iFieldType))
            {
            case CFT_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, s_constraintTypeNames, std::extent<decltype(s_constraintTypeNames)>::value);
                break;
                
            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromPhysConstraintsConverter(const PhysConstraints* structure, const cspField_t* fields, const size_t fieldCount, std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
}

InfoString AssetDumperPhysConstraints::CreateInfoString(XAssetInfo<PhysConstraints>* asset)
{
    assert(asset->Asset()->count <= 4);

    InfoStringFromPhysConstraintsConverter converter(asset->Asset(), phys_constraints_fields, std::extent<decltype(phys_constraints_fields)>::value, [asset](const scr_string_t scrStr) -> std::string
        {
            assert(scrStr < asset->m_zone->m_script_strings.Count());
            if (scrStr >= asset->m_zone->m_script_strings.Count())
                return "";

            return asset->m_zone->m_script_strings[scrStr];
        });

    return converter.Convert();
}

bool AssetDumperPhysConstraints::ShouldDump(XAssetInfo<PhysConstraints>* asset)
{
    return true;
}

bool AssetDumperPhysConstraints::CanDumpAsRaw()
{
    return true;
}

bool AssetDumperPhysConstraints::CanDumpAsGdtEntry()
{
    return true;
}

std::string AssetDumperPhysConstraints::GetFileNameForAsset(Zone* zone, XAssetInfo<PhysConstraints>* asset)
{
    return "physconstraints/" + asset->m_name;
}

GdtEntry AssetDumperPhysConstraints::DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<PhysConstraints>* asset)
{
    const auto infoString = CreateInfoString(asset);
    GdtEntry gdtEntry(asset->m_name, ObjConstants::GDF_FILENAME_PHYS_CONSTRAINTS);
    infoString.ToGdtProperties(ObjConstants::INFO_STRING_PREFIX_PHYS_CONSTRAINTS, gdtEntry);

    return gdtEntry;
}

void AssetDumperPhysConstraints::DumpRaw(AssetDumpingContext& context, XAssetInfo<PhysConstraints>* asset, std::ostream& stream)
{
    const auto infoString = CreateInfoString(asset);
    const auto stringValue = infoString.ToString(ObjConstants::INFO_STRING_PREFIX_PHYS_CONSTRAINTS);
    stream.write(stringValue.c_str(), stringValue.size());
}