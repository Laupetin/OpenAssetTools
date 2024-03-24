#include "AssetDumperWeaponAttachment.h"

#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringFromStructConverter.h"
#include "Game/T6/InfoString/WeaponAttachmentFields.h"
#include "Game/T6/ObjConstantsT6.h"

#include <cassert>
#include <type_traits>

using namespace T6;

namespace T6
{
    class InfoStringFromAttachmentConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<attachmentFieldType_t>(field.iFieldType))
            {
            case AFT_ATTACHMENTTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szAttachmentTypeNames, std::extent_v<decltype(szAttachmentTypeNames)>);
                break;

            case AFT_PENETRATE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, penetrateTypeNames, std::extent_v<decltype(penetrateTypeNames)>);
                break;

            case AFT_FIRETYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapFireTypeNames, std::extent_v<decltype(szWeapFireTypeNames)>);
                break;

            default:
                break;
            }
        }

    public:
        InfoStringFromAttachmentConverter(const WeaponAttachment* structure,
                                          const cspField_t* fields,
                                          const size_t fieldCount,
                                          std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
} // namespace T6

InfoString AssetDumperWeaponAttachment::CreateInfoString(XAssetInfo<WeaponAttachment>* asset)
{
    InfoStringFromAttachmentConverter converter(asset->Asset(),
                                                attachment_fields,
                                                std::extent_v<decltype(attachment_fields)>,
                                                [asset](const scr_string_t scrStr) -> std::string
                                                {
                                                    assert(scrStr < asset->m_zone->m_script_strings.Count());
                                                    if (scrStr >= asset->m_zone->m_script_strings.Count())
                                                        return "";

                                                    return asset->m_zone->m_script_strings[scrStr];
                                                });

    return converter.Convert();
}

bool AssetDumperWeaponAttachment::ShouldDump(XAssetInfo<WeaponAttachment>* asset)
{
    return true;
}

void AssetDumperWeaponAttachment::DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponAttachment>* asset)
{
    // Only dump raw when no gdt available
    if (context.m_gdt)
    {
        const auto infoString = CreateInfoString(asset);
        GdtEntry gdtEntry(asset->m_name, ObjConstants::GDF_FILENAME_WEAPON_ATTACHMENT);
        infoString.ToGdtProperties(ObjConstants::INFO_STRING_PREFIX_WEAPON_ATTACHMENT, gdtEntry);
        context.m_gdt->WriteEntry(gdtEntry);
    }
    else
    {
        const auto assetFile = context.OpenAssetFile("attachment/" + asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        const auto infoString = CreateInfoString(asset);
        const auto stringValue = infoString.ToString(ObjConstants::INFO_STRING_PREFIX_WEAPON_ATTACHMENT);
        stream.write(stringValue.c_str(), stringValue.size());
    }
}
