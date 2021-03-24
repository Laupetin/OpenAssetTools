#include "AssetDumperWeaponAttachment.h"

#include <cassert>
#include <sstream>
#include <type_traits>
#include <cstring>

#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringFromStructConverter.h"
#include "Game/T6/InfoString/WeaponAttachmentFields.h"

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
                FillFromEnumInt(std::string(field.szName), field.iOffset, szAttachmentTypeNames, std::extent<decltype(szAttachmentTypeNames)>::value);
                break;

            case AFT_PENETRATE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, penetrateTypeNames, std::extent<decltype(penetrateTypeNames)>::value);
                break;

            case AFT_FIRETYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapFireTypeNames, std::extent<decltype(szWeapFireTypeNames)>::value);
                break;

            default:
                break;
            }
        }

    public:
        InfoStringFromAttachmentConverter(const WeaponAttachment* structure, const cspField_t* fields, const size_t fieldCount, std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
}

InfoString AssetDumperWeaponAttachment::CreateInfoString(XAssetInfo<WeaponAttachment>* asset)
{
    InfoStringFromAttachmentConverter converter(asset->Asset(), attachment_fields, std::extent<decltype(attachment_fields)>::value, [asset](const scr_string_t scrStr) -> std::string
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

bool AssetDumperWeaponAttachment::CanDumpAsRaw()
{
    return true;
}

bool AssetDumperWeaponAttachment::CanDumpAsGdtEntry()
{
    return true;
}

std::string AssetDumperWeaponAttachment::GetFileNameForAsset(Zone* zone, XAssetInfo<WeaponAttachment>* asset)
{
    return "attachment/" + asset->m_name;
}

GdtEntry AssetDumperWeaponAttachment::DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<WeaponAttachment>* asset)
{
    const auto infoString = CreateInfoString(asset);
    GdtEntry gdtEntry(asset->m_name, GDF_NAME);
    infoString.ToGdtProperties(FILE_TYPE_STR, gdtEntry);

    return gdtEntry;
}

void AssetDumperWeaponAttachment::DumpRaw(AssetDumpingContext& context, XAssetInfo<WeaponAttachment>* asset, std::ostream& stream)
{
    const auto infoString = CreateInfoString(asset);
    const auto stringValue = infoString.ToString(FILE_TYPE_STR);
    stream.write(stringValue.c_str(), stringValue.size());
}
