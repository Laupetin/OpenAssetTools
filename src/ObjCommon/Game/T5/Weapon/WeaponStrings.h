#pragma once

#include "Game/T5/T5.h"

namespace T5
{
    inline const char* szWeapTypeNames[]{
        "bullet",
        "grenade",
        "projectile",
        "binoculars",
        "gas",
        "bomb",
        "mine",
        "melee",
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
        "melee",
        "Killstreak Alt Stored Weapon",
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
        "bullet_xtreme",
        "shotgun",
        "grenade_bounce",
        "grenade_explode",
        "rifle_grenade",
        "rocket_explode",
        "rocket_explode_xtreme",
        "projectile_dud",
        "mortar_shell",
        "tank_shell",
        "bolt",
        "blade",
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
        "bolt",
    };
    static_assert(std::extent_v<decltype(szProjectileExplosionNames)> == WEAPPROJEXP_NUM);

    inline const char* offhandClassNames[]{
        "None",
        "Frag Grenade",
        "Smoke Grenade",
        "Flash Grenade",
        "Gear",
    };
    static_assert(std::extent_v<decltype(offhandClassNames)> == OFFHAND_CLASS_NUM);

    inline const char* offhandSlotNames[]{
        "None",
        "Lethal grenade",
        "Tactical grenade",
        "Equipment",
        "Specific use",
    };
    static_assert(std::extent_v<decltype(offhandSlotNames)> == OFFHAND_SLOT_NUM);

    // mp/playeranimtypes.txt
    inline const char* playerAnimTypeNames[]{
        "none",     "default", "other",   "sniper", "m203",     "hold",           "briefcase",      "reviver", "radio",   "dualwield",  "remotecontrol",
        "crossbow", "minigun", "beltfed", "g11",    "rearclip", "rearclipsniper", "ballisticknife", "nopump",  "hatchet", "grimreaper", "zipline",
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
        "WireGuided",
        "TVGuided",
    };
    static_assert(std::extent_v<decltype(guidedMissileNames)> == MISSILE_GUIDANCE_NUM);

    inline const char* stickinessNames[]{
        "Don't stick",
        "Stick to all",
        "Stick to all, except ai and clients",
        "Stick to ground",
        "Stick to ground, maintain yaw",
        "Stick to flesh",
    };
    static_assert(std::extent_v<decltype(stickinessNames)> == WEAPSTICKINESS_NUM);

    inline const char* rotateTypeNames[]{
        "Rotate both axis, grenade style",
        "Rotate one axis, blade style",
        "Rotate like a cylinder",
    };
    static_assert(std::extent_v<decltype(rotateTypeNames)> == WEAPROTATE_NUM);

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
        "melee",
    };
    static_assert(std::extent_v<decltype(szWeapInventoryTypeNames)> == WEAPINVENTORY_NUM);

    inline const char* szWeapFireTypeNames[]{
        "Full Auto",
        "Single Shot",
        "2-Round Burst",
        "3-Round Burst",
        "4-Round Burst",
        "Stacked Fire",
        "Minigun",
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
} // namespace T5
