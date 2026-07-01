#pragma once

#include "Game/T4/T4.h"

#include <type_traits>

namespace T4
{
    inline const char* szWeapTypeNames[]{
        "bullet",
        "grenade",
        "projectile",
        "binoculars",
        "gas",
        "bomb",
        "mine",
    };
    static_assert(std::extent_v<decltype(szWeapTypeNames)> == WEAPTYPE_NUM);

    inline const char* szWeapClassNames[]{
        "rifle",
        "mg",
        "smg",
        "spread",
        "pistol",
        "grenade",
        "rocketlauncher",
        "turret",
        "non-player",
        "gas",
        "item",
    };
    static_assert(std::extent_v<decltype(szWeapClassNames)> == WEAPCLASS_NUM);

    inline const char* szWeapOverlayReticleNames[]{
        "none",
        "crosshair",
    };
    static_assert(std::extent_v<decltype(szWeapOverlayReticleNames)> == WEAPOVERLAYRETICLE_NUM);

    inline const char* penetrateTypeNames[]{
        "none",
        "small",
        "medium",
        "large",
    };
    static_assert(std::extent_v<decltype(penetrateTypeNames)> == PENETRATE_TYPE_NUM);

    inline const char* impactTypeNames[]{
        "none",
        "bullet_small",
        "bullet_large",
        "bullet_ap",
        "shotgun",
        "grenade_bounce",
        "grenade_explode",
        "rifle_grenade",
        "rocket_explode",
        "projectile_dud",
        "mortar_shell",
        "tank_shell",
    };
    static_assert(std::extent_v<decltype(impactTypeNames)> == IMPACT_TYPE_NUM);

    inline const char* szWeapStanceNames[]{
        "stand",
        "duck",
        "prone",
    };
    static_assert(std::extent_v<decltype(szWeapStanceNames)> == WEAPSTANCE_NUM);

    inline const char* szProjectileExplosionNames[]{
        "grenade",
        "rocket",
        "flashbang",
        "none",
        "dud",
        "smoke",
        "heavy explosive",
        "fire",
        "napalmblob",
    };
    static_assert(std::extent_v<decltype(szProjectileExplosionNames)> == WEAPPROJEXP_NUM);

    inline const char* offhandClassNames[]{
        "None",
        "Frag Grenade",
        "Smoke Grenade",
        "Flash Grenade",
    };
    static_assert(std::extent_v<decltype(offhandClassNames)> == OFFHAND_CLASS_NUM);

    // mp/playeranimtypes.txt
    inline const char* playerAnimTypeNames[]{
        "none",   "other", "pistol", "smg",  "autorifle", "mg",  "sniper",  "rocketlauncher", "explosive", "grenade",
        "turret", "c4",    "m203",   "hold", "briefcase", "gas", "revivee", "reviver",        "radio",
    };

    inline const char* activeReticleNames[]{
        "None",
        "Pip-On-A-Stick",
        "Bouncing diamond",
    };
    static_assert(std::extent_v<decltype(activeReticleNames)> == VEH_ACTIVE_RETICLE_NUM);

    inline const char* guidedMissileNames[]{
        "None",
        "Sidewinder",
        "Hellfire",
        "Javelin",
        "Ballistic",
    };
    static_assert(std::extent_v<decltype(guidedMissileNames)> == MISSILE_GUIDANCE_NUM);

    inline const char* stickinessNames[]{
        "Don't stick",
        "Stick to all",
        "Stick to ground",
        "Stick to ground, maintain yaw",
    };
    static_assert(std::extent_v<decltype(stickinessNames)> == WEAPSTICKINESS_NUM);

    inline const char* overlayInterfaceNames[]{
        "None",
        "Javelin",
        "Turret Scope",
    };
    static_assert(std::extent_v<decltype(overlayInterfaceNames)> == WEAPOVERLAYINTERFACE_NUM);

    inline const char* szWeapInventoryTypeNames[]{
        "primary",
        "offhand",
        "item",
        "altmode",
    };
    static_assert(std::extent_v<decltype(szWeapInventoryTypeNames)> == WEAPINVENTORY_NUM);

    inline const char* szWeapFireTypeNames[]{
        "Full Auto",
        "Single Shot",
        "2-Round Burst",
        "3-Round Burst",
        "4-Round Burst",
    };
    static_assert(std::extent_v<decltype(szWeapFireTypeNames)> == WEAPON_FIRETYPE_NUM);

    inline const char* szWeapClipTypeNames[]{
        "bottom",
        "top",
        "left",
        "dp28",
        "ptrs",
        "lmg",
    };
    static_assert(std::extent_v<decltype(szWeapClipTypeNames)> == WEAPON_CLIPTYPE_NUM);

    inline const char* ammoCounterClipNames[]{
        "None",
        "Magazine",
        "ShortMagazine",
        "Shotgun",
        "Rocket",
        "Beltfed",
        "AltWeapon",
    };
    static_assert(std::extent_v<decltype(ammoCounterClipNames)> == AMMO_COUNTER_CLIP_NUM);

    inline const char* weapIconRatioNames[]{
        "1:1",
        "2:1",
        "4:1",
    };
    static_assert(std::extent_v<decltype(weapIconRatioNames)> == WEAPON_ICON_RATIO_NUM);

    inline const char* bounceSoundSuffixes[]{
        "_default", "_bark",    "_brick",   "_carpet", "_cloth",   "_concrete", "_dirt",         "_flesh",  "_foliage",   "_glass", "_grass",
        "_gravel",  "_ice",     "_metal",   "_mud",    "_paper",   "_plaster",  "_rock",         "_sand",   "_snow",      "_water", "_wood",
        "_asphalt", "_ceramic", "_plastic", "_rubber", "_cushion", "_fruit",    "_paintedmetal", "_player", "_tallgrass",
    };
    static_assert(std::extent_v<decltype(bounceSoundSuffixes)> == SURF_TYPE_NUM);
} // namespace T4
