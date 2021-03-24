#include "AssetDumperWeapon.h"

#include <cassert>
#include <sstream>
#include <type_traits>
#include <cstring>

#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringFromStructConverter.h"
#include "Game/T6/InfoString/WeaponFields.h"

using namespace T6;

namespace T6
{
    class InfoStringFromWeaponConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<weapFieldType_t>(field.iFieldType))
            {
            case WFT_WEAPONTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapTypeNames, std::extent<decltype(szWeapTypeNames)>::value);
                break;

            case WFT_WEAPONCLASS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapClassNames, std::extent<decltype(szWeapClassNames)>::value);
                break;
            case WFT_OVERLAYRETICLE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapOverlayReticleNames, std::extent<decltype(szWeapOverlayReticleNames)>::value);
                break;

            case WFT_PENETRATE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, penetrateTypeNames, std::extent<decltype(penetrateTypeNames)>::value);
                break;

            case WFT_IMPACT_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, impactTypeNames, std::extent<decltype(impactTypeNames)>::value);
                break;

            case WFT_STANCE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapStanceNames,
                                std::extent<decltype(szWeapStanceNames)>::value);
                break;

            case WFT_PROJ_EXPLOSION:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szProjectileExplosionNames,
                                std::extent<decltype(szProjectileExplosionNames)>::value);
                break;

            case WFT_OFFHAND_CLASS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, offhandClassNames,
                                std::extent<decltype(offhandClassNames)>::value);
                break;

            case WFT_OFFHAND_SLOT:
                FillFromEnumInt(std::string(field.szName), field.iOffset, offhandSlotNames, std::extent<decltype(offhandSlotNames)>::value);
                break;

            case WFT_ANIMTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, playerAnimTypeNames,
                                std::extent<decltype(playerAnimTypeNames)>::value);
                break;

            case WFT_ACTIVE_RETICLE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, activeReticleNames,
                                std::extent<decltype(activeReticleNames)>::value);
                break;

            case WFT_GUIDED_MISSILE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, guidedMissileNames,
                                std::extent<decltype(guidedMissileNames)>::value);
                break;

            case WFT_BOUNCE_SOUND:
                {
                    const auto* bounceSound = *reinterpret_cast<const char***>(reinterpret_cast<uintptr_t>(m_structure)
                        + field.iOffset);

                    if (bounceSound && bounceSound[0])
                    {
                        const std::string firstBounceSound(bounceSound[0]);
                        const auto endOfBouncePrefix = firstBounceSound.rfind("_default");
                        assert(endOfBouncePrefix != std::string::npos);

                        if (endOfBouncePrefix != std::string::npos)
                        {
                            m_info_string.SetValueForKey(std::string(field.szName),
                                                         firstBounceSound.substr(0, endOfBouncePrefix));
                        }
                        else
                            m_info_string.SetValueForKey(std::string(field.szName), "");
                    }
                    else
                        m_info_string.SetValueForKey(std::string(field.szName), "");

                    break;
                }

            case WFT_STICKINESS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, stickinessNames, std::extent<decltype(stickinessNames)>::value);
                break;

            case WFT_ROTATETYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, rotateTypeNames, std::extent<decltype(rotateTypeNames)>::value);
                break;

            case WFT_OVERLAYINTERFACE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, overlayInterfaceNames,
                                std::extent<decltype(overlayInterfaceNames)>::value);
                break;

            case WFT_INVENTORYTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapInventoryTypeNames,
                                std::extent<decltype(szWeapInventoryTypeNames)>::value);
                break;

            case WFT_FIRETYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapFireTypeNames,
                                std::extent<decltype(szWeapFireTypeNames)>::value);
                break;

            case WFT_CLIPTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapClipTypeNames,
                                std::extent<decltype(szWeapClipTypeNames)>::value);
                break;

            case WFT_AMMOCOUNTER_CLIPTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, ammoCounterClipNames,
                                std::extent<decltype(ammoCounterClipNames)>::value);
                break;

            case WFT_ICONRATIO_HUD:
            case WFT_ICONRATIO_AMMOCOUNTER:
            case WFT_ICONRATIO_KILL:
            case WFT_ICONRATIO_DPAD:
            case WFT_ICONRATIO_INDICATOR:
                FillFromEnumInt(std::string(field.szName), field.iOffset, weapIconRatioNames,
                                std::extent<decltype(weapIconRatioNames)>::value);
                break;

            case WFT_BARRELTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, barrelTypeNames, std::extent<decltype(barrelTypeNames)>::value);
                break;

            case WFT_HIDETAGS:
                {
                    const auto* hideTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                    std::stringstream ss;
                    bool first = true;

                    for (auto i = 0u; i < std::extent<decltype(WeaponFullDef::hideTags)>::value; i++)
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

            case WFT_EXPLOSION_TAG:
                FillFromScriptString(std::string(field.szName), field.iOffset);
                break;

            case WFT_NOTETRACKSOUNDMAP:
                {
                    const auto* keys = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                    const auto* values = &keys[std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value];
                    std::stringstream ss;
                    bool first = true;

                    for (auto i = 0u; i < std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value; i++)
                    {
                        const auto& key = m_get_scr_string(keys[i]);
                        const auto& value = m_get_scr_string(values[i]);
                        if (!key.empty())
                        {
                            if (!first)
                                ss << "\n";
                            else
                                first = false;

                            ss << key;

                            if (!value.empty())
                                ss << " " << value;
                        }
                    }

                    m_info_string.SetValueForKey(std::string(field.szName), ss.str());
                    break;
                }

            case WFT_WEAPON_CAMO:
                {
                    const auto* camo = *reinterpret_cast<WeaponCamo**>(reinterpret_cast<uintptr_t>(m_structure) + field
                        .iOffset);

                    if (camo)
                        m_info_string.SetValueForKey(std::string(field.szName), std::string(AssetName(camo->name)));
                    else
                        m_info_string.SetValueForKey(std::string(field.szName), "");
                    break;
                }

            case WFT_NUM_FIELD_TYPES:
            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromWeaponConverter(const WeaponFullDef* structure, const cspField_t* fields, const size_t fieldCount, std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
}

void AssetDumperWeapon::CopyToFullDef(const WeaponVariantDef* weapon, WeaponFullDef* fullDef)
{
    fullDef->weapVariantDef = *weapon;

    if (weapon->weapDef)
    {
        fullDef->weapDef = *weapon->weapDef;
        fullDef->weapVariantDef.weapDef = &fullDef->weapDef;
    }

    if (weapon->attachments)
    {
        assert(sizeof(WeaponFullDef::attachments) >= sizeof(void*) * std::extent<decltype(WeaponFullDef::attachments)>::value);
        memcpy(fullDef->attachments, weapon->attachments, sizeof(void*) * std::extent<decltype(WeaponFullDef::attachments)>::value);
        fullDef->weapVariantDef.attachments = fullDef->attachments;
    }

    if (weapon->attachmentUniques)
    {
        assert(sizeof(WeaponFullDef::attachmentUniques) >= sizeof(void*) * std::extent<decltype(WeaponFullDef::attachmentUniques)>::value);
        memcpy(fullDef->attachmentUniques, weapon->attachmentUniques, sizeof(void*) * std::extent<decltype(WeaponFullDef::attachmentUniques)>::value);
        fullDef->weapVariantDef.attachmentUniques = fullDef->attachmentUniques;
    }

    if (fullDef->weapDef.gunXModel)
    {
        assert(sizeof(WeaponFullDef::gunXModel) >= sizeof(void*) * std::extent<decltype(WeaponFullDef::gunXModel)>::value);
        memcpy(fullDef->gunXModel, fullDef->weapDef.gunXModel, sizeof(void*) * std::extent<decltype(WeaponFullDef::gunXModel)>::value);
        fullDef->weapDef.gunXModel = fullDef->gunXModel;
    }

    if (weapon->szXAnims)
    {
        assert(sizeof(WeaponFullDef::szXAnims) >= sizeof(void*) * NUM_WEAP_ANIMS);
        memcpy(fullDef->szXAnims, weapon->szXAnims, sizeof(void*) * NUM_WEAP_ANIMS);
        fullDef->weapVariantDef.szXAnims = fullDef->szXAnims;
    }

    if (weapon->hideTags)
    {
        assert(sizeof(WeaponFullDef::hideTags) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::hideTags)>::value);
        memcpy(fullDef->hideTags, weapon->hideTags, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::hideTags)>::value);
        fullDef->weapVariantDef.hideTags = fullDef->hideTags;
    }

    if (fullDef->weapDef.notetrackSoundMapKeys)
    {
        assert(sizeof(WeaponFullDef::notetrackSoundMapKeys) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value);
        memcpy(fullDef->notetrackSoundMapKeys, fullDef->weapDef.notetrackSoundMapKeys, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value);
        fullDef->weapDef.notetrackSoundMapKeys = fullDef->notetrackSoundMapKeys;
    }

    if (fullDef->weapDef.notetrackSoundMapValues)
    {
        assert(sizeof(WeaponFullDef::notetrackSoundMapValues) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackSoundMapValues)>::value);
        memcpy(fullDef->notetrackSoundMapValues, fullDef->weapDef.notetrackSoundMapValues, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackSoundMapValues)>::value);
        fullDef->weapDef.notetrackSoundMapValues = fullDef->notetrackSoundMapValues;
    }

    if (fullDef->weapDef.worldModel)
    {
        assert(sizeof(WeaponFullDef::worldModel) >= sizeof(void*) * std::extent<decltype(WeaponFullDef::worldModel)>::value);
        memcpy(fullDef->worldModel, fullDef->weapDef.worldModel, sizeof(void*) * std::extent<decltype(WeaponFullDef::worldModel)>::value);
        fullDef->weapDef.worldModel = fullDef->worldModel;
    }

    if (weapon->attachViewModel)
    {
        assert(sizeof(WeaponFullDef::attachViewModel) >= sizeof(void*) * std::extent<decltype(WeaponFullDef::attachViewModel)>::value);
        memcpy(fullDef->attachViewModel, weapon->attachViewModel, sizeof(void*) * std::extent<decltype(WeaponFullDef::attachViewModel)>::value);
        fullDef->weapVariantDef.attachViewModel = fullDef->attachViewModel;
    }

    if (weapon->attachWorldModel)
    {
        assert(sizeof(WeaponFullDef::attachWorldModel) >= sizeof(void*) * std::extent<decltype(WeaponFullDef::attachWorldModel)>::value);
        memcpy(fullDef->attachWorldModel, weapon->attachWorldModel, sizeof(void*) * std::extent<decltype(WeaponFullDef::attachWorldModel)>::value);
        fullDef->weapVariantDef.attachWorldModel = fullDef->attachWorldModel;
    }

    if (weapon->attachViewModelTag)
    {
        assert(sizeof(WeaponFullDef::attachViewModelTag) >= sizeof(void*) * std::extent<decltype(WeaponFullDef::attachViewModelTag)>::value);
        memcpy(fullDef->attachViewModelTag, weapon->attachViewModelTag, sizeof(void*) * std::extent<decltype(WeaponFullDef::attachViewModelTag)>::value);
        fullDef->weapVariantDef.attachViewModelTag = fullDef->attachViewModelTag;
    }

    if (weapon->attachWorldModelTag)
    {
        assert(sizeof(WeaponFullDef::attachWorldModelTag) >= sizeof(void*) * std::extent<decltype(WeaponFullDef::attachWorldModelTag)>::value);
        memcpy(fullDef->attachWorldModelTag, weapon->attachWorldModelTag, sizeof(void*) * std::extent<decltype(WeaponFullDef::attachWorldModelTag)>::value);
        fullDef->weapVariantDef.attachWorldModelTag = fullDef->attachWorldModelTag;
    }

    if (fullDef->weapDef.parallelBounce)
    {
        assert(sizeof(WeaponFullDef::parallelBounce) >= sizeof(float) * SURF_TYPE_NUM);
        memcpy(fullDef->parallelBounce, fullDef->weapDef.parallelBounce, sizeof(float) * SURF_TYPE_NUM);
        fullDef->weapDef.parallelBounce = fullDef->parallelBounce;
    }

    if (fullDef->weapDef.perpendicularBounce)
    {
        assert(sizeof(WeaponFullDef::perpendicularBounce) >= sizeof(float) * SURF_TYPE_NUM);
        memcpy(fullDef->perpendicularBounce, fullDef->weapDef.perpendicularBounce, sizeof(float) * SURF_TYPE_NUM);
        fullDef->weapDef.perpendicularBounce = fullDef->perpendicularBounce;
    }

    if (fullDef->weapDef.locationDamageMultipliers)
    {
        assert(sizeof(WeaponFullDef::locationDamageMultipliers) >= sizeof(float) * HITLOC_NUM);
        memcpy(fullDef->locationDamageMultipliers, fullDef->weapDef.locationDamageMultipliers, sizeof(float) * HITLOC_NUM);
        fullDef->weapDef.locationDamageMultipliers = fullDef->locationDamageMultipliers;
    }

    if (fullDef->weapDef.weaponCamo && fullDef->weapDef.weaponCamo->name)
    {
        strncpy(fullDef->weaponCamo, fullDef->weapDef.weaponCamo->name, std::extent<decltype(WeaponFullDef::weaponCamo)>::value);
    }
}

InfoString AssetDumperWeapon::CreateInfoString(XAssetInfo<WeaponVariantDef>* asset)
{
    const auto fullDef = std::make_unique<WeaponFullDef>();
    memset(fullDef.get(), 0, sizeof(WeaponFullDef));
    CopyToFullDef(asset->Asset(), fullDef.get());

    InfoStringFromWeaponConverter converter(fullDef.get(), weapon_fields, std::extent<decltype(weapon_fields)>::value, [asset](const scr_string_t scrStr) -> std::string
    {
        assert(scrStr < asset->m_zone->m_script_strings.Count());
        if (scrStr >= asset->m_zone->m_script_strings.Count())
            return "";

        return asset->m_zone->m_script_strings[scrStr];
    });

    return converter.Convert();
}

bool AssetDumperWeapon::ShouldDump(XAssetInfo<WeaponVariantDef>* asset)
{
    return true;
}

bool AssetDumperWeapon::CanDumpAsRaw()
{
    return true;
}

bool AssetDumperWeapon::CanDumpAsGdtEntry()
{
    return true;
}

std::string AssetDumperWeapon::GetFileNameForAsset(Zone* zone, XAssetInfo<WeaponVariantDef>* asset)
{
    return "weapons/" + asset->m_name;
}

GdtEntry AssetDumperWeapon::DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<WeaponVariantDef>* asset)
{
    const auto infoString = CreateInfoString(asset);
    GdtEntry gdtEntry(asset->m_name, GDF_NAME);
    infoString.ToGdtProperties(FILE_TYPE_STR, gdtEntry);

    return gdtEntry;
}

void AssetDumperWeapon::DumpRaw(AssetDumpingContext& context, XAssetInfo<WeaponVariantDef>* asset, std::ostream& stream)
{
    const auto infoString = CreateInfoString(asset);
    const auto stringValue = infoString.ToString(FILE_TYPE_STR);
    stream.write(stringValue.c_str(), stringValue.size());
}
