#include "AttachmentDumperT6.h"

#include "Game/T6/InfoString/InfoStringFromStructConverter.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/Weapon/AttachmentFields.h"
#include "Game/T6/Weapon/WeaponStrings.h"
#include "Weapon/AttachmentCommon.h"

#include <cassert>
#include <type_traits>

using namespace T6;

namespace
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

    InfoString CreateInfoString(const XAssetInfo<WeaponAttachment>& asset)
    {
        InfoStringFromAttachmentConverter converter(asset.Asset(),
                                                    attachment_fields,
                                                    std::extent_v<decltype(attachment_fields)>,
                                                    [asset](const scr_string_t scrStr) -> std::string
                                                    {
                                                        assert(scrStr < asset.m_zone->m_script_strings.Count());
                                                        if (scrStr >= asset.m_zone->m_script_strings.Count())
                                                            return "";

                                                        return asset.m_zone->m_script_strings[scrStr];
                                                    });

        return converter.Convert();
    }
} // namespace

namespace attachment
{
    DumperT6::DumperT6(const AssetPool<AssetAttachment::Type>& pool)
        : AbstractAssetDumper(pool)
    {
    }

    void DumperT6::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetAttachment::Type>& asset)
    {
        // Only dump raw when no gdt available
        if (context.m_gdt)
        {
            const auto infoString = CreateInfoString(asset);
            GdtEntry gdtEntry(asset.m_name, ObjConstants::GDF_FILENAME_WEAPON_ATTACHMENT);
            infoString.ToGdtProperties(ObjConstants::INFO_STRING_PREFIX_WEAPON_ATTACHMENT, gdtEntry);
            context.m_gdt->WriteEntry(gdtEntry);
        }
        else
        {
            const auto assetFile = context.OpenAssetFile(GetInfoStringFileNameForAssetName(asset.m_name));

            if (!assetFile)
                return;

            auto& stream = *assetFile;
            const auto infoString = CreateInfoString(asset);
            const auto stringValue = infoString.ToString(ObjConstants::INFO_STRING_PREFIX_WEAPON_ATTACHMENT);
            stream.write(stringValue.c_str(), stringValue.size());
        }
    }
} // namespace attachment
