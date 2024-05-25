#include "AssetDumperWeaponAttachmentUnique.h"

#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringFromStructConverter.h"
#include "Game/T6/InfoString/WeaponAttachmentUniqueFields.h"
#include "Game/T6/ObjConstantsT6.h"

#include <cassert>
#include <cstring>
#include <sstream>
#include <type_traits>

using namespace T6;

namespace T6
{
    class InfoStringFromWeaponAttachmentUniqueConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<attachmentUniqueFieldType_t>(field.iFieldType))
            {
            case AUFT_ATTACHMENTTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szAttachmentTypeNames, std::extent_v<decltype(szAttachmentTypeNames)>);
                break;

            case AUFT_HIDETAGS:
            {
                const auto* hideTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                std::stringstream ss;
                auto first = true;

                for (auto i = 0u; i < std::extent_v<decltype(WeaponFullDef::hideTags)>; i++)
                {
                    const auto& str = m_get_scr_string(hideTags[i]);
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

            case AUFT_OVERLAYRETICLE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapOverlayReticleNames, std::extent_v<decltype(szWeapOverlayReticleNames)>);
                break;

            case AUFT_CAMO:
            {
                const auto* camo = *reinterpret_cast<WeaponCamo**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

                if (camo)
                    m_info_string.SetValueForKey(std::string(field.szName), std::string(AssetName(camo->name)));
                else
                    m_info_string.SetValueForKey(std::string(field.szName), "");
                break;
            }

            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromWeaponAttachmentUniqueConverter(const WeaponAttachmentUniqueFull* structure,
                                                      const cspField_t* fields,
                                                      const size_t fieldCount,
                                                      std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
} // namespace T6

void AssetDumperWeaponAttachmentUnique::CopyToFullDef(const WeaponAttachmentUnique* attachment, WeaponAttachmentUniqueFull* fullDef)
{
    fullDef->attachment = *attachment;

    if (attachment->szXAnims)
    {
        assert(sizeof(WeaponAttachmentUniqueFull::szXAnims) >= sizeof(void*) * NUM_WEAP_ANIMS);
        memcpy(fullDef->szXAnims, attachment->szXAnims, sizeof(void*) * NUM_WEAP_ANIMS);
        fullDef->attachment.szXAnims = fullDef->szXAnims;
    }

    if (attachment->hideTags)
    {
        assert(sizeof(WeaponAttachmentUniqueFull::hideTags) >= sizeof(scr_string_t) * std::extent_v<decltype(WeaponAttachmentUniqueFull::hideTags)>);
        memcpy(fullDef->hideTags, attachment->hideTags, sizeof(scr_string_t) * std::extent_v<decltype(WeaponAttachmentUniqueFull::hideTags)>);
        fullDef->attachment.hideTags = fullDef->hideTags;
    }

    if (attachment->locationDamageMultipliers)
    {
        assert(sizeof(WeaponAttachmentUniqueFull::locationDamageMultipliers) >= sizeof(float) * HITLOC_COUNT);
        memcpy(fullDef->locationDamageMultipliers, attachment->locationDamageMultipliers, sizeof(float) * HITLOC_COUNT);
        fullDef->attachment.locationDamageMultipliers = fullDef->locationDamageMultipliers;
    }
}

InfoString AssetDumperWeaponAttachmentUnique::CreateInfoString(XAssetInfo<WeaponAttachmentUnique>* asset)
{
    const auto fullDef = std::make_unique<WeaponAttachmentUniqueFull>();
    memset(fullDef.get(), 0, sizeof(WeaponAttachmentUniqueFull));
    CopyToFullDef(asset->Asset(), fullDef.get());

    InfoStringFromWeaponAttachmentUniqueConverter converter(fullDef.get(),
                                                            attachment_unique_fields,
                                                            std::extent_v<decltype(attachment_unique_fields)>,
                                                            [asset](const scr_string_t scrStr) -> std::string
                                                            {
                                                                assert(scrStr < asset->m_zone->m_script_strings.Count());
                                                                if (scrStr >= asset->m_zone->m_script_strings.Count())
                                                                    return "";

                                                                return asset->m_zone->m_script_strings[scrStr];
                                                            });

    return converter.Convert();
}

bool AssetDumperWeaponAttachmentUnique::ShouldDump(XAssetInfo<WeaponAttachmentUnique>* asset)
{
    return true;
}

void AssetDumperWeaponAttachmentUnique::DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponAttachmentUnique>* asset)
{
    // Only dump raw when no gdt available
    if (context.m_gdt)
    {
        const auto infoString = CreateInfoString(asset);
        GdtEntry gdtEntry(asset->m_name, ObjConstants::GDF_FILENAME_WEAPON_ATTACHMENT_UNIQUE);
        infoString.ToGdtProperties(ObjConstants::INFO_STRING_PREFIX_WEAPON_ATTACHMENT_UNIQUE, gdtEntry);
        context.m_gdt->WriteEntry(gdtEntry);
    }
    else
    {
        const auto assetFile = context.OpenAssetFile("attachmentunique/" + asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        const auto infoString = CreateInfoString(asset);
        const auto stringValue = infoString.ToString(ObjConstants::INFO_STRING_PREFIX_WEAPON_ATTACHMENT_UNIQUE);
        stream.write(stringValue.c_str(), stringValue.size());
    }
}
