#include "AssetDumperWeapon.h"

#include <cassert>
#include <cstring>
#include <sstream>
#include <type_traits>

#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/ObjConstantsIW4.h"
#include "Game/IW4/InfoString/EnumStrings.h"
#include "Game/IW4/InfoString/InfoStringFromStructConverter.h"
#include "Game/IW4/InfoString/WeaponFields.h"

using namespace IW4;

namespace IW4
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
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapStanceNames, std::extent<decltype(szWeapStanceNames)>::value);
                break;

            case WFT_PROJ_EXPLOSION:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szProjectileExplosionNames, std::extent<decltype(szProjectileExplosionNames)>::value);
                break;

            case WFT_OFFHAND_CLASS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, offhandClassNames, std::extent<decltype(offhandClassNames)>::value);
                break;

            case WFT_ANIMTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, playerAnimTypeNames, std::extent<decltype(playerAnimTypeNames)>::value);
                break;

            case WFT_ACTIVE_RETICLE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, activeReticleNames, std::extent<decltype(activeReticleNames)>::value);
                break;

            case WFT_GUIDED_MISSILE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, guidedMissileNames, std::extent<decltype(guidedMissileNames)>::value);
                break;

            case WFT_BOUNCE_SOUND:
                {
                    const auto* bounceSound = *reinterpret_cast<SndAliasCustom**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

                    if (bounceSound && bounceSound->name)
                    {
                        const std::string firstBounceSound(bounceSound->name->soundName);
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

            case WFT_OVERLAYINTERFACE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, overlayInterfaceNames, std::extent<decltype(overlayInterfaceNames)>::value);
                break;

            case WFT_INVENTORYTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapInventoryTypeNames, std::extent<decltype(szWeapInventoryTypeNames)>::value);
                break;

            case WFT_FIRETYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapFireTypeNames, std::extent<decltype(szWeapFireTypeNames)>::value);
                break;

            case WFT_AMMOCOUNTER_CLIPTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, ammoCounterClipNames, std::extent<decltype(ammoCounterClipNames)>::value);
                break;

            case WFT_ICONRATIO_HUD:
            case WFT_ICONRATIO_PICKUP:
            case WFT_ICONRATIO_AMMOCOUNTER:
            case WFT_ICONRATIO_KILL:
            case WFT_ICONRATIO_DPAD:
                FillFromEnumInt(std::string(field.szName), field.iOffset, weapIconRatioNames, std::extent<decltype(weapIconRatioNames)>::value);
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

            case WFT_NOTETRACKRUMBLEMAP:
                {
                    const auto* keys = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                    const auto* values = &keys[std::extent<decltype(WeaponFullDef::notetrackRumbleMapKeys)>::value];
                    std::stringstream ss;
                    bool first = true;

                    for (auto i = 0u; i < std::extent<decltype(WeaponFullDef::notetrackRumbleMapKeys)>::value; i++)
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

void AssetDumperWeapon::CopyToFullDef(const WeaponCompleteDef* weapon, WeaponFullDef* fullDef)
{
    fullDef->weapCompleteDef = *weapon;

    if (weapon->weapDef)
    {
        fullDef->weapDef = *weapon->weapDef;
        fullDef->weapCompleteDef.weapDef = &fullDef->weapDef;
    }

    if (weapon->hideTags)
    {
        assert(sizeof(WeaponFullDef::hideTags) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::hideTags)>::value);
        memcpy(fullDef->hideTags, weapon->hideTags, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::hideTags)>::value);
        fullDef->weapCompleteDef.hideTags = fullDef->hideTags;
    }

    if (weapon->szXAnims)
    {
        assert(sizeof(WeaponFullDef::szXAnims) >= sizeof(void*) * NUM_WEAP_ANIMS);
        memcpy(fullDef->szXAnims, weapon->szXAnims, sizeof(void*) * NUM_WEAP_ANIMS);
        fullDef->weapCompleteDef.szXAnims = fullDef->szXAnims;
    }

    if (fullDef->weapDef.gunXModel)
    {
        assert(sizeof(WeaponFullDef::gunXModel) >= sizeof(void*) * std::extent<decltype(WeaponFullDef::gunXModel)>::value);
        memcpy(fullDef->gunXModel, fullDef->weapDef.gunXModel, sizeof(void*) * std::extent<decltype(WeaponFullDef::gunXModel)>::value);
        fullDef->weapDef.gunXModel = fullDef->gunXModel;
    }

    if (fullDef->weapDef.szXAnimsRightHanded)
    {
        assert(sizeof(WeaponFullDef::szXAnimsRightHanded) >= sizeof(void*) * NUM_WEAP_ANIMS);
        memcpy(fullDef->szXAnimsRightHanded, fullDef->weapDef.szXAnimsRightHanded, sizeof(void*) * NUM_WEAP_ANIMS);
        fullDef->weapDef.szXAnimsRightHanded = fullDef->szXAnimsRightHanded;
    }

    if (fullDef->weapDef.szXAnimsLeftHanded)
    {
        assert(sizeof(WeaponFullDef::szXAnimsLeftHanded) >= sizeof(void*) * NUM_WEAP_ANIMS);
        memcpy(fullDef->szXAnimsLeftHanded, fullDef->weapDef.szXAnimsLeftHanded, sizeof(void*) * NUM_WEAP_ANIMS);
        fullDef->weapDef.szXAnimsLeftHanded = fullDef->szXAnimsLeftHanded;
    }

    if (fullDef->weapDef.notetrackSoundMapKeys)
    {
        assert(sizeof(WeaponFullDef::notetrackSoundMapKeys) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value);
        memcpy(fullDef->notetrackSoundMapKeys, fullDef->weapDef.notetrackSoundMapKeys, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value);
        fullDef->weapDef.notetrackSoundMapKeys = fullDef->notetrackSoundMapKeys;
    }

    if (fullDef->weapDef.notetrackSoundMapValues)
    {
        assert(sizeof(WeaponFullDef::notetrackSoundMapValues) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value);
        memcpy(fullDef->notetrackSoundMapValues, fullDef->weapDef.notetrackSoundMapValues, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackSoundMapKeys)>::value);
        fullDef->weapDef.notetrackSoundMapValues = fullDef->notetrackSoundMapValues;
    }

    if (fullDef->weapDef.notetrackRumbleMapKeys)
    {
        assert(sizeof(WeaponFullDef::notetrackRumbleMapKeys) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackRumbleMapKeys)>::value);
        memcpy(fullDef->notetrackRumbleMapKeys, fullDef->weapDef.notetrackRumbleMapKeys, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackRumbleMapKeys)>::value);
        fullDef->weapDef.notetrackRumbleMapKeys = fullDef->notetrackRumbleMapKeys;
    }

    if (fullDef->weapDef.notetrackRumbleMapValues)
    {
        assert(sizeof(WeaponFullDef::notetrackRumbleMapValues) >= sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackRumbleMapKeys)>::value);
        memcpy(fullDef->notetrackRumbleMapValues, fullDef->weapDef.notetrackRumbleMapValues, sizeof(scr_string_t) * std::extent<decltype(WeaponFullDef::notetrackRumbleMapKeys)>::value);
        fullDef->weapDef.notetrackRumbleMapValues = fullDef->notetrackRumbleMapValues;
    }

    if (fullDef->weapDef.worldModel)
    {
        assert(sizeof(WeaponFullDef::worldModel) >= sizeof(void*) * std::extent<decltype(WeaponFullDef::worldModel)>::value);
        memcpy(fullDef->worldModel, fullDef->weapDef.worldModel, sizeof(void*) * std::extent<decltype(WeaponFullDef::worldModel)>::value);
        fullDef->weapDef.worldModel = fullDef->worldModel;
    }

    if (fullDef->weapDef.parallelBounce)
    {
        assert(sizeof(WeaponFullDef::parallelBounce) >= sizeof(float) * std::extent<decltype(WeaponFullDef::parallelBounce)>::value);
        memcpy(fullDef->parallelBounce, fullDef->weapDef.parallelBounce, sizeof(float) * std::extent<decltype(WeaponFullDef::parallelBounce)>::value);
        fullDef->weapDef.parallelBounce = fullDef->parallelBounce;
    }

    if (fullDef->weapDef.perpendicularBounce)
    {
        assert(sizeof(WeaponFullDef::perpendicularBounce) >= sizeof(float) * std::extent<decltype(WeaponFullDef::perpendicularBounce)>::value);
        memcpy(fullDef->perpendicularBounce, fullDef->weapDef.perpendicularBounce, sizeof(float) * std::extent<decltype(WeaponFullDef::perpendicularBounce)>::value);
        fullDef->weapDef.perpendicularBounce = fullDef->perpendicularBounce;
    }

    if (fullDef->weapDef.locationDamageMultipliers)
    {
        assert(sizeof(WeaponFullDef::locationDamageMultipliers) >= sizeof(float) * std::extent<decltype(WeaponFullDef::locationDamageMultipliers)>::value);
        memcpy(fullDef->locationDamageMultipliers, fullDef->weapDef.locationDamageMultipliers, sizeof(float) * std::extent<decltype(WeaponFullDef::locationDamageMultipliers)>::value);
        fullDef->weapDef.locationDamageMultipliers = fullDef->locationDamageMultipliers;
    }
}

InfoString AssetDumperWeapon::CreateInfoString(XAssetInfo<WeaponCompleteDef>* asset)
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

bool AssetDumperWeapon::ShouldDump(XAssetInfo<WeaponCompleteDef>* asset)
{
    return true;
}

void AssetDumperWeapon::DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponCompleteDef>* asset)
{
    // Only dump raw when no gdt available
    if (context.m_gdt)
    {
        const auto infoString = CreateInfoString(asset);
        GdtEntry gdtEntry(asset->m_name, ObjConstants::GDF_FILENAME_WEAPON);
        infoString.ToGdtProperties(ObjConstants::INFO_STRING_PREFIX_WEAPON, gdtEntry);
        context.m_gdt->WriteEntry(gdtEntry);
    }
    else
    {
        const auto assetFile = context.OpenAssetFile("weapons/" + asset->m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        const auto infoString = CreateInfoString(asset);
        const auto stringValue = infoString.ToString(ObjConstants::INFO_STRING_PREFIX_WEAPON);
        stream.write(stringValue.c_str(), stringValue.size());
    }
}
