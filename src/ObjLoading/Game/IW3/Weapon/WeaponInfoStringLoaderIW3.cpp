#include "WeaponInfoStringLoaderIW3.h"

#include "Game/IW3/IW3.h"
#include "Game/IW3/InfoString/InfoStringToStructConverter.h"
#include "Game/IW3/Weapon/WeaponFields.h"
#include "Game/IW3/Weapon/WeaponStrings.h"
#include "Utils/Logging/Log.h"
#include "Weapon/WeaponCommon.h"

#include <array>
#include <cassert>
#include <cstring>
#include <type_traits>
#include <vector>

using namespace IW3;

namespace
{
    class InfoStringToWeaponConverter final : public InfoStringToStructConverter
    {
        bool ConvertHideTags(const cspField_t& field, const std::string& value)
        {
            std::vector<std::string> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                con::error("Failed to parse hide tags as array");
                return false;
            }

            if (valueArray.size() > std::extent_v<decltype(WeaponDef::hideTags)>)
            {
                con::error("Cannot have more than {} hide tags!", std::extent_v<decltype(WeaponDef::hideTags)>);
                return false;
            }

            auto* hideTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

            if (valueArray.size() < std::extent_v<decltype(WeaponDef::hideTags)>)
                m_registration.AddScriptString(m_zone_script_strings.AddOrGetScriptString(nullptr));

            auto currentHideTag = 0u;
            for (; currentHideTag < valueArray.size(); currentHideTag++)
            {
                const auto& currentValue = valueArray[currentHideTag];
                const auto scrString =
                    !currentValue.empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue) : m_zone_script_strings.AddOrGetScriptString(nullptr);
                hideTags[currentHideTag] = scrString;
                m_registration.AddScriptString(scrString);
            }

            for (; currentHideTag < std::extent_v<decltype(WeaponDef::hideTags)>; currentHideTag++)
                hideTags[currentHideTag] = m_zone_script_strings.GetScriptString(nullptr);

            return true;
        }

        [[nodiscard]] bool ConvertBounceSounds(const cspField_t& field, const std::string& value) const
        {
            auto** bounceSound = reinterpret_cast<SndAliasCustom**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            if (value.empty())
            {
                *bounceSound = nullptr;
                return true;
            }

            assert(std::extent_v<decltype(bounceSoundSuffixes)> == SURF_TYPE_NUM);
            *bounceSound = m_memory.Alloc<SndAliasCustom>(SURF_TYPE_NUM);
            for (auto i = 0u; i < SURF_TYPE_NUM; i++)
            {
                const auto currentBounceSound = value + bounceSoundSuffixes[i];

                (*bounceSound)[i].name = m_memory.Alloc<snd_alias_list_name>();
                (*bounceSound)[i].name->soundName = m_memory.Dup(currentBounceSound.c_str());
            }

            return true;
        }

        bool ConvertNotetrackSoundMap(const cspField_t& field, const std::string& value)
        {
            std::vector<std::array<std::string, 2>> pairs;
            if (!ParseAsArray(value, pairs))
            {
                con::error("Failed to parse notetracksoundmap as pairs");
                return false;
            }

            if (pairs.size() > std::extent_v<decltype(WeaponDef::notetrackSoundMapKeys)>)
            {
                con::error("Cannot have more than {} notetracksoundmap entries!", std::extent_v<decltype(WeaponDef::notetrackSoundMapKeys)>);
                return false;
            }

            auto* keys = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            auto* values = &keys[std::extent_v<decltype(WeaponDef::notetrackSoundMapKeys)>];
            auto currentEntryNum = 0u;

            if (pairs.size() < std::extent_v<decltype(WeaponDef::notetrackSoundMapKeys)>)
                m_registration.AddScriptString(m_zone_script_strings.AddOrGetScriptString(nullptr));

            for (; currentEntryNum < pairs.size(); currentEntryNum++)
            {
                const auto& currentValue = pairs[currentEntryNum];
                const auto keyScriptString = !currentValue[0].empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue[0])
                                                                      : m_zone_script_strings.AddOrGetScriptString(nullptr);
                const auto valueScriptString = !currentValue[1].empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue[1])
                                                                        : m_zone_script_strings.AddOrGetScriptString(nullptr);

                keys[currentEntryNum] = keyScriptString;
                m_registration.AddScriptString(keyScriptString);

                values[currentEntryNum] = valueScriptString;
                m_registration.AddScriptString(valueScriptString);
            }

            for (; currentEntryNum < std::extent_v<decltype(WeaponDef::notetrackSoundMapKeys)>; currentEntryNum++)
            {
                const auto emptyScr = m_zone_script_strings.GetScriptString(nullptr);
                keys[currentEntryNum] = emptyScr;
                values[currentEntryNum] = emptyScr;
            }

            return true;
        }

        bool ConvertAnimName(const cspField_t& field, const std::string& value)
        {
            if (ConvertString(value, field.iOffset))
            {
                if (!value.empty())
                    m_registration.AddIndirectAssetReference(m_context.LoadIndirectAssetReference<AssetXAnim>(value));

                return true;
            }

            return false;
        }

    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<weapFieldType_t>(field.iFieldType))
            {
            case WFT_WEAPONTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapTypeNames, std::extent_v<decltype(szWeapTypeNames)>);

            case WFT_WEAPONCLASS:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapClassNames, std::extent_v<decltype(szWeapClassNames)>);

            case WFT_OVERLAYRETICLE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapOverlayReticleNames, std::extent_v<decltype(szWeapOverlayReticleNames)>);

            case WFT_PENETRATE_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, penetrateTypeNames, std::extent_v<decltype(penetrateTypeNames)>);

            case WFT_IMPACT_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, impactTypeNames, std::extent_v<decltype(impactTypeNames)>);

            case WFT_STANCE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapStanceNames, std::extent_v<decltype(szWeapStanceNames)>);

            case WFT_PROJ_EXPLOSION:
                return ConvertEnumInt(field.szName, value, field.iOffset, szProjectileExplosionNames, std::extent_v<decltype(szProjectileExplosionNames)>);

            case WFT_OFFHAND_CLASS:
                return ConvertEnumInt(field.szName, value, field.iOffset, offhandClassNames, std::extent_v<decltype(offhandClassNames)>);

            case WFT_ANIMTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, playerAnimTypeNames, std::extent_v<decltype(playerAnimTypeNames)>);

            case WFT_ACTIVE_RETICLE_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, activeReticleNames, std::extent_v<decltype(activeReticleNames)>);

            case WFT_GUIDED_MISSILE_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, guidedMissileNames, std::extent_v<decltype(guidedMissileNames)>);

            case WFT_BOUNCE_SOUND:
                return ConvertBounceSounds(field, value);

            case WFT_STICKINESS:
                return ConvertEnumInt(field.szName, value, field.iOffset, stickinessNames, std::extent_v<decltype(stickinessNames)>);

            case WFT_OVERLAYINTERFACE:
                return ConvertEnumInt(field.szName, value, field.iOffset, overlayInterfaceNames, std::extent_v<decltype(overlayInterfaceNames)>);

            case WFT_INVENTORYTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapInventoryTypeNames, std::extent_v<decltype(szWeapInventoryTypeNames)>);

            case WFT_FIRETYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapFireTypeNames, std::extent_v<decltype(szWeapFireTypeNames)>);

            case WFT_AMMOCOUNTER_CLIPTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, ammoCounterClipNames, std::extent_v<decltype(ammoCounterClipNames)>);

            case WFT_ICONRATIO_HUD:
            case WFT_ICONRATIO_AMMOCOUNTER:
            case WFT_ICONRATIO_KILL:
            case WFT_ICONRATIO_DPAD:
                return ConvertEnumInt(field.szName, value, field.iOffset, weapIconRatioNames, std::extent_v<decltype(weapIconRatioNames)>);

            case WFT_HIDETAGS:
                return ConvertHideTags(field, value);

            case WFT_NOTETRACKSOUNDMAP:
                return ConvertNotetrackSoundMap(field, value);

            case WFT_ANIM_NAME:
                return ConvertAnimName(field, value);

            default:
                assert(false);
                return false;
            }
        }

    public:
        InfoStringToWeaponConverter(const InfoString& infoString,
                                    WeaponDef& weaponDef,
                                    ZoneScriptStrings& zoneScriptStrings,
                                    MemoryManager& memory,
                                    AssetCreationContext& context,
                                    AssetRegistration<AssetWeapon>& registration,
                                    const cspField_t* fields,
                                    const size_t fieldCount)
            : InfoStringToStructConverter(infoString, &weaponDef, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
        }
    };

    void InitWeaponDef(WeaponDef& weapon)
    {
        for (const auto& field : weapon_fields)
        {
            if (field.iFieldType != CSPFT_STRING && field.iFieldType != WFT_ANIM_NAME)
                continue;

            *reinterpret_cast<const char**>(reinterpret_cast<char*>(&weapon) + field.iOffset) = "";
        }

        weapon.szXAnims[WEAP_ANIM_ROOT] = "";
    }

    bool LoadAccuracyGraph(const std::string& graphName,
                           vec2_t*& originalGraphKnots,
                           int& originalGraphKnotCount,
                           vec2_t*& graphKnots,
                           int& graphKnotCount,
                           AssetCreationContext& context)
    {
        auto* accuracyGraphAsset = context.LoadSubAsset<SubAssetAccuracyGraph>(graphName);
        if (!accuracyGraphAsset)
            return false;

        const auto* accuracyGraph = accuracyGraphAsset->Asset();

        assert(accuracyGraphAsset->m_dependencies.empty());
        assert(accuracyGraphAsset->m_used_script_strings.empty());
        assert(accuracyGraphAsset->m_indirect_asset_references.empty());

        originalGraphKnots = accuracyGraph->graphKnots;
        originalGraphKnotCount = accuracyGraph->graphKnotCount;

        graphKnots = accuracyGraph->graphKnots;
        graphKnotCount = accuracyGraph->graphKnotCount;

        return true;
    }

    bool LoadAccuracyGraphs(WeaponDef& weaponDef, AssetCreationContext& context)
    {
        if (weaponDef.aiVsAiAccuracyGraphName && weaponDef.aiVsAiAccuracyGraphName[0])
        {
            if (!LoadAccuracyGraph(weapon::GetAssetNameForAiVsAiAccuracyGraph(weaponDef.aiVsAiAccuracyGraphName),
                                   weaponDef.originalAiVsAiAccuracyGraphKnots,
                                   weaponDef.originalAiVsAiAccuracyGraphKnotCount,
                                   weaponDef.aiVsAiAccuracyGraphKnots,
                                   weaponDef.aiVsAiAccuracyGraphKnotCount,
                                   context))
            {
                return false;
            }
        }

        if (weaponDef.aiVsPlayerAccuracyGraphName && weaponDef.aiVsPlayerAccuracyGraphName[0])
        {
            if (!LoadAccuracyGraph(weapon::GetAssetNameForAiVsPlayerAccuracyGraph(weaponDef.aiVsPlayerAccuracyGraphName),
                                   weaponDef.originalAiVsPlayerAccuracyGraphKnots,
                                   weaponDef.originalAiVsPlayerAccuracyGraphKnotCount,
                                   weaponDef.aiVsPlayerAccuracyGraphKnots,
                                   weaponDef.aiVsPlayerAccuracyGraphKnotCount,
                                   context))
            {
                return false;
            }
        }

        return true;
    }

    bool IsDefaultWeapon(const WeaponDef& weapon)
    {
        return strcmp(weapon.szInternalName, "defaultweapon") == 0 || strcmp(weapon.szInternalName, "defaultweapon_mp") == 0;
    }

    snd_alias_list_name* SetDefaultSound(const char* name, MemoryManager& memory)
    {
        auto* aliasListName = memory.Alloc<snd_alias_list_name>();
        aliasListName->soundName = name;

        return aliasListName;
    }

    void SetWeaponDefaults(WeaponDef& weapon, MemoryManager& memory)
    {
        if (IsDefaultWeapon(weapon))
            return;

        if (!weapon.viewLastShotEjectEffect)
            weapon.viewLastShotEjectEffect = weapon.viewShellEjectEffect;
        if (!weapon.worldLastShotEjectEffect)
            weapon.worldLastShotEjectEffect = weapon.worldShellEjectEffect;
        if (!weapon.raiseSound.name)
            SetDefaultSound("weap_raise", memory);
        if (!weapon.putawaySound.name)
            SetDefaultSound("weap_putaway", memory);
        if (!weapon.pickupSound.name)
            SetDefaultSound("weap_pickup", memory);
        if (!weapon.ammoPickupSound.name)
            SetDefaultSound("weap_ammo_pickup", memory);
        if (!weapon.emptyFireSound.name)
            SetDefaultSound("weap_dryfire_smg_npc", memory);
    }

    void SetupTransitionTimes(WeaponDef& weapon)
    {
        if (weapon.iAdsTransInTime <= 0)
            weapon.fOOPosAnimLength[0] = 1.0f / 300.0f; // 0.0033333334f;
        else
            weapon.fOOPosAnimLength[0] = 1.0f / static_cast<float>(weapon.iAdsTransInTime);

        if (weapon.iAdsTransOutTime <= 0)
            weapon.fOOPosAnimLength[1] = 1.0f / 500.0f; // 0.0020000001f
        else
            weapon.fOOPosAnimLength[1] = 1.0f / static_cast<float>(weapon.iAdsTransOutTime);
    }

    void CheckWeaponDamageRanges(WeaponDef& weapon)
    {
        if (strcmp(weapon.szInternalName, "none") == 0)
            return;

        if (weapon.fMaxDamageRange <= 0.0)
            weapon.fMaxDamageRange = 999999.0f;
        if (weapon.fMinDamageRange <= 0.0)
            weapon.fMinDamageRange = 999999.12f; // oddly specific number, no clue
    }

    void CheckCrosshairValues(WeaponDef& weapon)
    {
        if (weapon.enemyCrosshairRange > 15000.0f)
            con::warn("Weapon {}: Enemy crosshair ranges should be less than 15000", weapon.szInternalName);
    }

    void CheckProjectileValues(WeaponDef& weapon)
    {
        if (weapon.weapType != WEAPTYPE_PROJECTILE)
            return;

        if (weapon.iProjectileSpeed <= 0)
            con::warn("Weapon {}: Projectile speed must be greater than 0.0", weapon.szDisplayName);

        if (weapon.destabilizationCurvatureMax >= 1000000000.0f || weapon.destabilizationCurvatureMax < 0.0f)
            con::warn("Weapon {}: Destabilization angle must be between 0 and 45 degrees", weapon.szDisplayName);

        if (weapon.destabilizationRateTime < 0.0f)
            con::warn("Weapon {}: Destabilization rate time must be non-negative", weapon.szDisplayName);
    }

    void CheckSharedAmmoValues(const WeaponDef& weapon)
    {
        if (weapon.szAmmoName)
            utils::MakeStringLowerCase(const_cast<char*>(weapon.szAmmoName));

        if (weapon.szClipName)
            utils::MakeStringLowerCase(const_cast<char*>(weapon.szClipName));
    }
} // namespace

namespace weapon
{
    InfoStringLoaderIW3::InfoStringLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
        : m_memory(memory),
          m_search_path(searchPath),
          m_zone(zone)
    {
    }

    AssetCreationResult InfoStringLoaderIW3::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context) const
    {
        auto* weaponDef = m_memory.Alloc<WeaponDef>();
        memset(weaponDef, 0, sizeof(WeaponDef));

        InitWeaponDef(*weaponDef);
        weaponDef->szInternalName = m_memory.Dup(assetName.c_str());

        AssetRegistration<AssetWeapon> registration(assetName, weaponDef);

        InfoStringToWeaponConverter converter(
            infoString, *weaponDef, m_zone.m_script_strings, m_memory, context, registration, weapon_fields, std::extent_v<decltype(weapon_fields)>);
        if (!converter.Convert())
        {
            con::error("Failed to parse weapon: \"{}\"", assetName);
            return AssetCreationResult::Failure();
        }

        if (!LoadAccuracyGraphs(*weaponDef, context))
        {
            con::error("Failed to load accuracy tables of weapon: \"{}\"", assetName);
            return AssetCreationResult::Failure();
        }

        SetWeaponDefaults(*weaponDef, m_memory);
        SetupTransitionTimes(*weaponDef);
        CheckWeaponDamageRanges(*weaponDef);
        CheckCrosshairValues(*weaponDef);
        CheckProjectileValues(*weaponDef);
        CheckSharedAmmoValues(*weaponDef);

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }
} // namespace weapon
