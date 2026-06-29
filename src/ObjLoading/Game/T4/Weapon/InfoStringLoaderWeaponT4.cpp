#include "InfoStringLoaderWeaponT4.h"

#include "Game/T4/InfoString/InfoStringToStructConverter.h"
#include "Game/T4/T4.h"
#include "Game/T4/Weapon/WeaponFields.h"
#include "Game/T4/Weapon/WeaponStrings.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"
#include "Weapon/WeaponCommon.h"

#include <cassert>
#include <cstring>
#include <sstream>
#include <type_traits>
#include <vector>

using namespace T4;

namespace
{
    class InfoStringToWeaponConverter final : public InfoStringToStructConverter
    {
        bool ConvertHideTags(const cspField_t& field, const std::string& value)
        {
            std::istringstream ss(value);
            std::vector<std::string> tags;

            std::string tag;
            while (ss >> tag)
                tags.emplace_back(std::move(tag));

            if (tags.size() > std::extent_v<decltype(WeaponDef::hideTags)>)
            {
                con::error("Cannot have more than {} hide tags!", std::extent_v<decltype(WeaponDef::hideTags)>);
                return false;
            }

            auto* hideTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

            if (tags.size() < std::extent_v<decltype(WeaponDef::hideTags)>)
                m_registration.AddScriptString(m_zone_script_strings.AddOrGetScriptString(nullptr));

            auto currentHideTag = 0u;
            for (; currentHideTag < tags.size(); currentHideTag++)
            {
                auto currentValue = tags[currentHideTag];
                utils::MakeStringLowerCase(currentValue);

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

            static_assert(std::extent_v<decltype(bounceSoundSuffixes)> == std::extent_v<decltype(WeaponDef::parallelBounce)>);
            *bounceSound = m_memory.Alloc<SndAliasCustom>(std::extent_v<decltype(bounceSoundSuffixes)>);
            for (auto i = 0u; i < std::extent_v<decltype(bounceSoundSuffixes)>; i++)
            {
                const auto currentBounceSound = value + bounceSoundSuffixes[i];

                (*bounceSound)[i].name = m_memory.Alloc<snd_alias_list_name>();
                (*bounceSound)[i].name->soundName = m_memory.Dup(currentBounceSound.c_str());
                m_registration.AddIndirectAssetReference(m_context.LoadIndirectAssetReference<AssetSound>(currentBounceSound));
            }

            return true;
        }

        [[nodiscard]] bool ConvertNotetrackSoundMap(const cspField_t& field, const std::string& value)
        {
            std::istringstream ss(value);
            std::vector<std::string> tokens;

            std::string token;
            while (ss >> token)
                tokens.emplace_back(std::move(token));

            if (tokens.size() % 2 != 0)
            {
                con::warn("Notetrack-to-Sound: Weapon '{}' has bad entry; notetrack '{}' doesn't have a corresponding sound.",
                          *reinterpret_cast<const char**>(m_structure),
                          tokens.back());
                tokens.pop_back();
            }

            const auto pairCount = tokens.size() / 2;
            if (pairCount > std::extent_v<decltype(WeaponDef::notetrackSoundMapKeys)>)
            {
                con::error("Cannot have more than {} notetracksoundmap entries!", std::extent_v<decltype(WeaponDef::notetrackSoundMapKeys)>);
                return false;
            }

            auto* keys = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            auto* values = &keys[std::extent_v<decltype(WeaponDef::notetrackSoundMapKeys)>];
            auto currentEntryNum = 0u;

            if (pairCount < std::extent_v<decltype(WeaponDef::notetrackSoundMapKeys)>)
                m_registration.AddScriptString(m_zone_script_strings.AddOrGetScriptString(nullptr));

            for (; currentEntryNum < pairCount; currentEntryNum++)
            {
                auto key = tokens[currentEntryNum * 2];
                auto sound = tokens[currentEntryNum * 2 + 1];

                if (key.size() >= 63)
                {
                    con::error("Notetrack-to-sound: keyname \"{}\" is too long (length {}/{}).", key, key.size(), 63);
                    return false;
                }

                utils::MakeStringLowerCase(key);
                utils::MakeStringLowerCase(sound);

                const auto keyScriptString =
                    !key.empty() ? m_zone_script_strings.AddOrGetScriptString(key) : m_zone_script_strings.AddOrGetScriptString(nullptr);
                const auto valueScriptString =
                    !sound.empty() ? m_zone_script_strings.AddOrGetScriptString(sound) : m_zone_script_strings.AddOrGetScriptString(nullptr);

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

            case WFT_CLIPTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapClipTypeNames, std::extent_v<decltype(szWeapClipTypeNames)>);

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

    bool LoadAccuracyGraphs(WeaponDef& weapon, AssetCreationContext& context)
    {
        if (weapon.aiVsAiAccuracyGraphName && weapon.aiVsAiAccuracyGraphName[0])
        {
            if (!LoadAccuracyGraph(weapon::GetAssetNameForAiVsAiAccuracyGraph(weapon.aiVsAiAccuracyGraphName),
                                   weapon.originalAiVsAiAccuracyGraphKnots,
                                   weapon.originalAiVsAiAccuracyGraphKnotCount,
                                   weapon.aiVsAiAccuracyGraphKnots,
                                   weapon.aiVsAiAccuracyGraphKnotCount,
                                   context))
            {
                return false;
            }
        }

        if (weapon.aiVsPlayerAccuracyGraphName && weapon.aiVsPlayerAccuracyGraphName[0])
        {
            if (!LoadAccuracyGraph(weapon::GetAssetNameForAiVsPlayerAccuracyGraph(weapon.aiVsPlayerAccuracyGraphName),
                                   weapon.originalAiVsPlayerAccuracyGraphKnots,
                                   weapon.originalAiVsPlayerAccuracyGraphKnotCount,
                                   weapon.aiVsPlayerAccuracyGraphKnots,
                                   weapon.aiVsPlayerAccuracyGraphKnotCount,
                                   context))
            {
                return false;
            }
        }

        return true;
    }

    bool LoadFlameTable(const char* flameTableName, FlameTable*& flameTablePtr, AssetRegistration<AssetWeapon>& registration, AssetCreationContext& context)
    {
        flameTablePtr = nullptr;
        if (!flameTableName || flameTableName[0] == '\0')
            return true;

        auto* flameTableAsset = context.LoadSubAsset<SubAssetFlameTable>(flameTableName);
        if (!flameTableAsset)
            return false;

        for (auto* dependency : flameTableAsset->m_dependencies)
            registration.AddDependency(dependency);

        assert(flameTableAsset->m_used_script_strings.empty());
        for (const auto& indirectReference : flameTableAsset->m_indirect_asset_references)
            registration.AddIndirectAssetReference(indirectReference);

        flameTablePtr = flameTableAsset->Asset();
        return true;
    }

    bool LoadFlameTables(WeaponDef& weapon, AssetRegistration<AssetWeapon>& registration, AssetCreationContext& context)
    {
        return LoadFlameTable(weapon.flameTableFirstPerson, weapon.flameTableFirstPersonPtr, registration, context)
               && LoadFlameTable(weapon.flameTableThirdPerson, weapon.flameTableThirdPersonPtr, registration, context);
    }

    bool IsDefaultWeapon(const WeaponDef& weapon)
    {
        return strcmp(weapon.szInternalName, "defaultweapon") == 0 || strcmp(weapon.szInternalName, "defaultweapon_mp") == 0;
    }

    void SetSoundAliasIfMissing(
        SndAliasCustom& sound, const char* aliasName, MemoryManager& memory, AssetCreationContext& context, AssetRegistration<AssetWeapon>& registration)
    {
        if (sound.name)
            return;

        sound.name = memory.Alloc<snd_alias_list_name>();
        sound.name->soundName = memory.Dup(aliasName);
        registration.AddIndirectAssetReference(context.LoadIndirectAssetReference<AssetSound>(aliasName));
    }

    void SetWeaponDefaults(WeaponDef& weapon, MemoryManager& memory, AssetCreationContext& context, AssetRegistration<AssetWeapon>& registration)
    {
        if (IsDefaultWeapon(weapon))
            return;

        if (!weapon.viewLastShotEjectEffect)
            weapon.viewLastShotEjectEffect = weapon.viewShellEjectEffect;
        if (!weapon.worldLastShotEjectEffect)
            weapon.worldLastShotEjectEffect = weapon.worldShellEjectEffect;

        SetSoundAliasIfMissing(weapon.raiseSound, "weap_raise", memory, context, registration);
        SetSoundAliasIfMissing(weapon.putawaySound, "weap_putaway", memory, context, registration);
        SetSoundAliasIfMissing(weapon.pickupSound, "weap_pickup", memory, context, registration);
        SetSoundAliasIfMissing(weapon.ammoPickupSound, "weap_ammo_pickup", memory, context, registration);
        SetSoundAliasIfMissing(weapon.emptyFireSound, "player_out_of_ammo", memory, context, registration);
    }

    void SetupTransitionTimes(WeaponDef& weapon)
    {
        if (weapon.iAdsTransInTime <= 0)
            weapon.fOOPosAnimLength[0] = 1.0f / 300.0f;
        else
            weapon.fOOPosAnimLength[0] = 1.0f / static_cast<float>(weapon.iAdsTransInTime);

        if (weapon.iAdsTransOutTime <= 0)
            weapon.fOOPosAnimLength[1] = 1.0f / 500.0f;
        else
            weapon.fOOPosAnimLength[1] = 1.0f / static_cast<float>(weapon.iAdsTransOutTime);
    }

    void CheckWeaponDamageRanges(WeaponDef& weapon)
    {
        if (weapon.fMaxDamageRange <= 0.0f)
            weapon.fMaxDamageRange = 999999.0f;
        if (weapon.fMinDamageRange <= 0.0f)
            weapon.fMinDamageRange = 999999.12f;
    }

    void CheckCrosshairValues(const WeaponDef& weapon)
    {
        if (weapon.enemyCrosshairRange > 15000.0f)
            con::error("Enemy crosshair ranges should be less than {}", 15000.0f);
    }

    void CheckProjectileValues(const WeaponDef& weapon)
    {
        if (weapon.weapType != WEAPTYPE_PROJECTILE)
            return;

        if (weapon.iProjectileSpeed <= 0)
            con::error("Projectile speed for WeapType {} must be greater than 0.0", weapon.szDisplayName);
        if (weapon.destabilizationCurvatureMax >= 1000000000.0f || weapon.destabilizationCurvatureMax < 0.0f)
            con::error("Destabilization angle for for WeapType {} must be between 0 and 45 degrees", weapon.szDisplayName);
        if (weapon.destabilizationRateTime < 0.0f)
            con::error("Destabilization rate time for for WeapType {} must be non-negative", weapon.szDisplayName);
    }

    void LowercaseAmmoNames(WeaponDef& weapon)
    {
        if (weapon.szAmmoName)
            utils::MakeStringLowerCase(const_cast<char*>(weapon.szAmmoName));

        if (weapon.szClipName)
            utils::MakeStringLowerCase(const_cast<char*>(weapon.szClipName));
    }
} // namespace

namespace weapon
{
    InfoStringLoaderT4::InfoStringLoaderT4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
        : m_memory(memory),
          m_search_path(searchPath),
          m_zone(zone)
    {
    }

    AssetCreationResult InfoStringLoaderT4::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
    {
        auto* weaponDef = m_memory.Alloc<WeaponDef>();
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

        if (!LoadFlameTables(*weaponDef, registration, context))
        {
            con::error("Failed to load flame tables of weapon: \"{}\"", assetName);
            return AssetCreationResult::Failure();
        }

        SetWeaponDefaults(*weaponDef, m_memory, context, registration);
        SetupTransitionTimes(*weaponDef);
        CheckWeaponDamageRanges(*weaponDef);
        CheckCrosshairValues(*weaponDef);
        CheckProjectileValues(*weaponDef);
        LowercaseAmmoNames(*weaponDef);

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }
} // namespace weapon
