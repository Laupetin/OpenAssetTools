#pragma once

namespace T6
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
        "riotshield",
    };

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
        "pistol spread",
    };

    inline const char* szWeapOverlayReticleNames[]{
        "none",
        "crosshair",
    };

    inline const char* penetrateTypeNames[]{
        "none",
        "small",
        "medium",
        "large",
    };

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

    inline const char* szWeapStanceNames[]{
        "stand",
        "duck",
        "prone",
    };

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
        "shrapnel span",
    };

    inline const char* offhandClassNames[]{
        "None",
        "Frag Grenade",
        "Smoke Grenade",
        "Flash Grenade",
        "Gear",
        "Supply Drop Marker",
    };

    inline const char* offhandSlotNames[]{
        "None",
        "Lethal grenade",
        "Tactical grenade",
        "Equipment",
        "Specific use",
    };

    inline const char* playerAnimTypeNames[]{
        "none",          "default",  "other",      "sniper",  "m203",       "hold",     "briefcase",  "reviver",        "radio",          "dualwield",
        "remotecontrol", "crossbow", "minigun",    "beltfed", "g11",        "rearclip", "handleclip", "rearclipsniper", "ballisticknife", "singleknife",
        "nopump",        "hatchet",  "grimreaper", "zipline", "riotshield", "tablet",   "turned",     "screecher",      "staff",
    };

    inline const char* activeReticleNames[]{
        "None",
        "Pip-On-A-Stick",
        "Bouncing Diamond",
        "Missile Lock",
    };

    inline const char* guidedMissileNames[]{
        "None",
        "Sidewinder",
        "Hellfire",
        "Javelin",
        "Ballistic",
        "WireGuided",
        "TVGuided",
        "Drone",
        "HeatSeeking",
    };

    inline const char* stickinessNames[]{
        "Don't stick",
        "Stick to all",
        "Stick to all, except ai and clients",
        "Stick to ground",
        "Stick to ground, maintain yaw",
        "Stick to flesh",
    };

    inline const char* rotateTypeNames[]{
        "Rotate both axis, grenade style",
        "Rotate one axis, blade style",
        "Rotate like a cylinder",
    };

    inline const char* overlayInterfaceNames[]{
        "None",
        "Javelin",
        "Turret Scope",
    };

    inline const char* szWeapInventoryTypeNames[]{
        "primary",
        "offhand",
        "item",
        "altmode",
        "melee",
        "dwlefthand",
    };

    inline const char* szWeapFireTypeNames[]{
        "Full Auto",
        "Single Shot",
        "2-Round Burst",
        "3-Round Burst",
        "4-Round Burst",
        "5-Round Burst",
        "Stacked Fire",
        "Minigun",
        "Charge Shot",
        "Jetgun",
    };

    inline const char* szWeapClipTypeNames[]{
        "bottom",
        "top",
        "left",
        "dp28",
        "ptrs",
        "lmg",
    };

    inline const char* ammoCounterClipNames[]{
        "None",
        "Magazine",
        "ShortMagazine",
        "Shotgun",
        "Rocket",
        "Beltfed",
        "AltWeapon",
    };

    inline const char* weapIconRatioNames[]{
        "1:1",
        "2:1",
        "4:1",
    };

    inline const char* barrelTypeNames[]{
        "Single",
        "Dual Barrel",
        "Dual Barrel Alternate",
        "Quad Barrel",
        "Quad Barrel Alternate",
        "Quad Barrel Double Alternate",
    };

    inline const char* bounceSoundSuffixes[]{
        "_default", "_bark",    "_brick",   "_carpet", "_cloth",   "_concrete", "_dirt",         "_flesh",  "_foliage",   "_glass",      "_grass",
        "_gravel",  "_ice",     "_metal",   "_mud",    "_paper",   "_plaster",  "_rock",         "_sand",   "_snow",      "_water",      "_wood",
        "_asphalt", "_ceramic", "_plastic", "_rubber", "_cushion", "_fruit",    "_paintedmetal", "_player", "_tallgrass", "_riotshield",
    };

    inline const char* szAttachmentTypeNames[]{
        "none",   "acog", "dualclip", "dualoptic", "dw",        "extbarrel", "extclip",    "extramags", "fastads",  "fastreload",
        "fmj",    "gl",   "grip",     "holo",      "ir",        "is",        "longbreath", "mk",        "mms",      "rangefinder",
        "reflex", "rf",   "sf",       "silencer",  "stackfire", "stalker",   "steadyaim",  "swayreduc", "tacknife", "vzoom",
    };
} // namespace T6
