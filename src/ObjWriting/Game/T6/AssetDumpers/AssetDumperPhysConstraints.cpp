#include "AssetDumperPhysConstraints.h"

#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringFromStructConverter.h"
#include "Game/T6/InfoString/PhysConstraintsFields.h"
#include "Game/T6/ObjConstantsT6.h"

#include <cassert>
#include <type_traits>

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
                FillFromEnumInt(std::string(field.szName), field.iOffset, s_constraintTypeNames, std::extent_v<decltype(s_constraintTypeNames)>);
                break;

            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromPhysConstraintsConverter(const PhysConstraints* structure,
                                               const cspField_t* fields,
                                               const size_t fieldCount,
                                               std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
} // namespace T6

InfoString AssetDumperPhysConstraints::CreateInfoString(XAssetInfo<PhysConstraints>* asset)
{
    assert(asset->Asset()->count <= 4);

    InfoStringFromPhysConstraintsConverter converter(asset->Asset(),
                                                     phys_constraints_fields,
                                                     std::extent_v<decltype(phys_constraints_fields)>,
                                                     [asset](const scr_string_t scrStr) -> std::string
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

void AssetDumperPhysConstraints::DumpAsset(AssetDumpingContext& context, XAssetInfo<PhysConstraints>* asset)
{
    // Only dump raw when no gdt available
    if (context.m_gdt)
    {
        const auto infoString = CreateInfoString(asset);
        GdtEntry gdtEntry(asset->m_name, ObjConstants::GDF_FILENAME_PHYS_CONSTRAINTS);
        infoString.ToGdtProperties(ObjConstants::INFO_STRING_PREFIX_PHYS_CONSTRAINTS, gdtEntry);
        context.m_gdt->WriteEntry(gdtEntry);
    }
    else
    {
        const auto assetFile = context.OpenAssetFile("physconstraints/" + asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        const auto infoString = CreateInfoString(asset);
        const auto stringValue = infoString.ToString(ObjConstants::INFO_STRING_PREFIX_PHYS_CONSTRAINTS);
        stream.write(stringValue.c_str(), stringValue.size());
    }
}
