#pragma once

namespace IW4
{
    inline const char* szWeapTypeNames[]{
        "bullet",
        "grenade",
        "projectile",
        "riotshield",
    };

    inline const char* szWeapClassNames[]{
        "rifle",
        "sniper",
        "mg",
        "smg",
        "spread",
        "pistol",
        "grenade",
        "rocketlauncher",
        "turret",
        "throwingknife",
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
        "exclusive",
        "scavenger",
    };

    inline const char* szWeapFireTypeNames[]{
        "Full Auto",
        "Single Shot",
        "2-Round Burst",
        "3-Round Burst",
        "4-Round Burst",
        "Double Barrel",
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
        "bullet_explode",
        "shotgun",
        "shotgun_explode",
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
        "Throwing Knife",
        "Other",
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
        "explosive",
        "grenade",
        "turret",
        "c4",
        "m203",
        "hold",
        "briefcase",
        "riotshield",
        "laptop",
        "throwingknife",
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
        "Stick to all, orient to surface",
        "Stick to ground",
        "Stick to ground, maintain yaw",
        "Knife",
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

    inline const char* s_vehicleClassNames[]{
        "4 wheel",
        "tank",
        "plane",
        "boat",
        "artillery",
        "helicopter",
        "snowmobile",
    };

    inline const char* s_vehicleAxleTypeNames[]{
        "front",
        "rear",
        "all",
    };
} // namespace IW4
