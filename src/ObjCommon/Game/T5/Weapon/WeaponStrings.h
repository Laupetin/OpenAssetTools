#pragma once

namespace T5
{
    inline const char* s_attachmentPointNames[5]{"none", "top", "bottom", "trigger", "muzzle"};

    inline const char* szAttachmentTypeNames[24]{"none", "reflex",   "elbit",   "acog",    "lps",      "vzoom",        "ir",   "gl",
                                             "mk",   "silencer", "grip",    "extclip",
                                      "dualclip", "saddle", "fmj",   "hp",   "rf",  "ft",    "bayonet", "auto", "speed", "upgradesight", "snub", "dw"};
    
    inline const char* s_attachmentGroupNames[3]{"attachment", "weaponoption", "point"};
    
    inline const char* s_weaponOptionGroupNames[6]{"camo", "reticle", "lens", "tag", "emblem", "reticle_color"};
    
    inline const char* szWeapTypeNames[8]{"bullet", "grenade", "projectile", "binoculars", "gas", "bomb", "mine", "melee"};
    
    inline const char* szWeapClassNames[13]{
        "rifle", "mg", "smg", "spread", "pistol", "grenade", "rocketlauncher", "turret", "non-player", "gas", "item", "melee", "Killstreak Alt Stored Weapon"};
    
    inline const char* szWeapInventoryTypeNames[5]{"primary", "offhand", "item", "altmode", "melee"};
    
    inline const char* szWeapFireTypeNames[7]{"Full Auto", "Single Shot", "2-Round Burst", "3-Round Burst", "4-Round Burst", "Stacked Fire", "Minigun"};
    
    inline const char* szWeapClipTypeNames[6]{"bottom", "top", "left", "dp28", "ptrs", "lmg"};
    
    inline const char* penetrateTypeNames[4]{"none", "small", "medium", "large"};
    
    inline const char* impactTypeNames[16]{"none",
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
                                      "blade"};
    inline const char* szProjectileExplosionNames[10]{
        "grenade", "rocket", "flashbang", "none", "dud", "smoke", "heavy explosive", "fire", "napalmblob", "bolt"};
    
    inline const char* szWeapOverlayReticleNames[2]{"none", "crosshair"};
    
    inline const char* szWeapStanceNames[3]{"stand", "duck", "prone"};
    
    inline const char* offhandClassNames[5]{"None", "Frag Grenade", "Smoke Grenade", "Flash Grenade", "Gear"};
    
    inline const char* offhandSlotNames[5]{"None", "Lethal grenade", "Tactical grenade", "Equipment", "Specific use"};
    
    inline const char* activeReticleNames[3]{"None", "Pip-On-A-Stick", "Bouncing diamond"};
    
    inline const char* guidedMissileNames[7]{"None", "Sidewinder", "Hellfire", "Javelin", "Ballistic", "WireGuided", "TVGuided"};
    
    inline const char* stickinessNames[6]{
        "Don't stick", "Stick to all", "Stick to all, except ai and clients", "Stick to ground", "Stick to ground, maintain yaw", "Stick to flesh"};
    
    inline const char* rotateTypeNames[3]{"Rotate both axis, grenade style", "Rotate one axis, blade style", "Rotate like a cylinder"};
    
    inline const char* overlayInterfaceNames[3]{"None", "Javelin", "Turret Scope"};
    
    inline const char* ammoCounterClipNames[7]{"None", "Magazine", "ShortMagazine", "Shotgun", "Rocket", "Beltfed", "AltWeapon"};
    
    inline const char* weapIconRatioNames[3]{"1:1", "2:1", "4:1"};

    inline const char* bounceSoundSuffixes[]{"_default", "_bark",   "_brick",   "_carpet", "_cloth",        "_concrete", "_dirt",     "_flesh",
                                             "_foliage", "_glass",  "_grass",   "_gravel", "_ice",          "_metal",    "_mud",      "_paper",
                                             "_plaster", "_rock",   "_sand",    "_snow",   "_water",        "_wood",     "_asphalt",  "_ceramic",
                                             "_plastic", "_rubber", "_cushion", "_fruit",  "_paintedmetal", "_player",   "_tallgrass"};

    inline const char* playerAnimTypeNames[]{
        "none",     "default", "other",   "sniper", "m203",     "hold",           "briefcase",      "reviver", "radio",   "dualwield",  "remotecontrol",
        "crossbow", "minigun", "beltfed", "g11",    "rearclip", "rearclipsniper", "ballisticknife", "nopump",  "hatchet", "grimreaper", "zipline ",
    };

    // TODO: This is from BO2 and not verified
    inline const char* barrelTypeNames[]{
        "Single",
        "Dual Barrel",
        "Dual Barrel Alternate",
        "Quad Barrel",
        "Quad Barrel Alternate",
        "Quad Barrel Double Alternate",
    };
} // namespace T5



