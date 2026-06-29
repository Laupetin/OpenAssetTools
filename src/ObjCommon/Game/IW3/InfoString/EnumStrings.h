#pragma once

namespace IW3
{
    inline const char* szWeapTypeNames[]{
        "bullet",
        "grenade",
        "projectile",
        "binoculars",
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
        "item",
    };

    inline const char* szWeapOverlayReticleNames[]{
        "none",
        "crosshair",
    };

    inline const char* szWeapInventoryTypeNames[]{
        "primary",
        "offhand",
        "item",
        "altmode",
    };

    inline const char* szWeapFireTypeNames[]{
        "Full Auto",
        "Single Shot",
        "2-Round Burst",
        "3-Round Burst",
        "4-Round Burst",
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
        "shotgun",
        "grenade_bounce",
        "grenade_explode",
        "rocket_explode",
        "projectile_dud",
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
    };

    inline const char* offhandClassNames[]{
        "None",
        "Frag Grenade",
        "Smoke Grenade",
        "Flash Grenade",
    };

    inline const char* playerAnimTypeNames[]{
        "none",
        "other",
        "pistol",
        "smg",
        "autorifle",
        "mg",
        "sniper",
        "rocketlauncher",
        "grenade",
        "c4",
        "m203",
        "hold",
        "briefcase",
    };

    inline const char* activeReticleNames[]{
        "None",
        "Pip-On-A-Stick",
        "Bouncing diamond",
    };

    inline const char* guidedMissileNames[]{
        "None",
        "Sidewinder",
        "Hellfire",
        "Javelin",
    };

    inline const char* stickinessNames[]{
        "Don't stick",
        "Stick to all",
        "Stick to ground",
        "Stick to ground, maintain yaw",
    };

    inline const char* overlayInterfaceNames[]{
        "None",
        "Javelin",
        "Turret Scope",
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

    inline const char* bounceSoundSuffixes[]{
        "_default", "_bark",   "_brick",   "_carpet",  "_cloth",   "_concrete", "_dirt",    "_flesh", "_foliage",       "_glass",
        "_grass",   "_gravel", "_ice",     "_metal",   "_mud",     "_paper",    "_plaster", "_rock",  "_sand",          "_snow",
        "_water",   "_wood",   "_asphalt", "_ceramic", "_plastic", "_rubber",   "_cushion", "_fruit", "_painted_metal",
    };
} // namespace IW3
