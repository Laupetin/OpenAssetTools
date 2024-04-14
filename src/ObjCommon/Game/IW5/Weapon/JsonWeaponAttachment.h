#pragma once

#include "Game/IW5/IW5.h"

#include "Json/JsonCommon.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace IW5
{
    NLOHMANN_JSON_SERIALIZE_ENUM(PenetrateType,
                                 {
                                     {PENETRATE_TYPE_NONE,   "none"  },
                                     {PENETRATE_TYPE_SMALL,  "small" },
                                     {PENETRATE_TYPE_MEDIUM, "medium"},
                                     {PENETRATE_TYPE_LARGE,  "large" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(ImpactType,
                                 {
                                     {IMPACT_TYPE_NONE,            "none"          },
                                     {IMPACT_TYPE_BULLET_SMALL,    "bulletSmall"   },
                                     {IMPACT_TYPE_BULLET_LARGE,    "bulletLarge"   },
                                     {IMPACT_TYPE_BULLET_AP,       "bulletAp"      },
                                     {IMPACT_TYPE_BULLET_EXPLODE,  "bulletExplode" },
                                     {IMPACT_TYPE_SHOTGUN,         "shotgun"       },
                                     {IMPACT_TYPE_SHOTGUN_EXPLODE, "shotgunExplode"},
                                     {IMPACT_TYPE_GRENADE_BOUNCE,  "grenadeBounce" },
                                     {IMPACT_TYPE_GRENADE_EXPLODE, "grenadeExplode"},
                                     {IMPACT_TYPE_ROCKET_EXPLODE,  "rocketExplode" },
                                     {IMPACT_TYPE_PROJECTILE_DUD,  "projectileDud" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(weapFireType_t,
                                 {
                                     {WEAPON_FIRETYPE_FULLAUTO,     "fullauto"    },
                                     {WEAPON_FIRETYPE_SINGLESHOT,   "singleshot"  },
                                     {WEAPON_FIRETYPE_BURSTFIRE2,   "burstfire2"  },
                                     {WEAPON_FIRETYPE_BURSTFIRE3,   "burstfire3"  },
                                     {WEAPON_FIRETYPE_BURSTFIRE4,   "burstfire4"  },
                                     {WEAPON_FIRETYPE_DOUBLEBARREL, "doublebarrel"},
    });

    class JsonAttAmmoGeneral
    {
    public:
        PenetrateType penetrateType;
        float penetrateMultiplier;
        ImpactType impactType;
        weapFireType_t fireType;
        std::optional<std::string> tracerType;
        bool rifleBullet;
        bool armorPiercing;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttAmmoGeneral, penetrateType, penetrateMultiplier, impactType, fireType, tracerType, rifleBullet, armorPiercing);

    class JsonAttSight
    {
    public:
        bool aimDownSight;
        bool adsFire;
        bool rechamberWhileAds;
        bool noAdsWhenMagEmpty;
        bool canHoldBreath;
        bool canVariableZoom;
        bool hideRailWithThisScope;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(
        JsonAttSight, aimDownSight, adsFire, rechamberWhileAds, noAdsWhenMagEmpty, canHoldBreath, canVariableZoom, hideRailWithThisScope);

    class JsonAttReload
    {
    public:
        bool noPartialReload;
        bool segmentedReload;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttReload, noPartialReload, segmentedReload);

    class JsonAttAddOns
    {
    public:
        bool motionTracker;
        bool silenced;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttAddOns, motionTracker, silenced);

    class JsonAttGeneral
    {
    public:
        bool boltAction;
        bool inheritsPerks;
        float enemyCrosshairRange;
        std::optional<std::string> reticleCenter;
        std::optional<std::string> reticleSide;
        int reticleCenterSize;
        int reticleSideSize;
        float moveSpeedScale;
        float adsMoveSpeedScale;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttGeneral,
                                   boltAction,
                                   inheritsPerks,
                                   enemyCrosshairRange,
                                   reticleCenter,
                                   reticleSide,
                                   reticleCenterSize,
                                   reticleSideSize,
                                   moveSpeedScale,
                                   adsMoveSpeedScale);

    class JsonAttAimAssist
    {
    public:
        float autoAimRange;
        float aimAssistRange;
        float aimAssistRangeAds;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttAimAssist, autoAimRange, aimAssistRange, aimAssistRangeAds);

    class JsonAttAmmunition
    {
    public:
        int maxAmmo;
        int startAmmo;
        int clipSize;
        int shotCount;
        int reloadAmmoAdd;
        int reloadStartAdd;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttAmmunition, maxAmmo, startAmmo, clipSize, shotCount, reloadAmmoAdd, reloadStartAdd);

    class JsonAttDamage
    {
    public:
        int damage;
        int minDamage;
        int meleeDamage;
        float maxDamageRange;
        float minDamageRange;
        int playerDamage;
        int minPlayerDamage;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttDamage, damage, minDamage, meleeDamage, maxDamageRange, minDamageRange, playerDamage, minPlayerDamage);

    class JsonAttLocationDamage
    {
    public:
        float locNone;
        float locHelmet;
        float locHead;
        float locNeck;
        float locTorsoUpper;
        float locTorsoLower;
        float locRightArmUpper;
        float locRightArmLower;
        float locRightHand;
        float locLeftArmUpper;
        float locLeftArmLower;
        float locLeftHand;
        float locRightLegUpper;
        float locRightLegLower;
        float locRightFoot;
        float locLeftLegUpper;
        float locLeftLegLower;
        float locLeftFoot;
        float locGun;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttLocationDamage,
                                   locNone,
                                   locHelmet,
                                   locHead,
                                   locNeck,
                                   locTorsoUpper,
                                   locTorsoLower,
                                   locRightArmUpper,
                                   locRightArmLower,
                                   locRightHand,
                                   locLeftArmUpper,
                                   locLeftArmLower,
                                   locLeftHand,
                                   locRightLegUpper,
                                   locRightLegLower,
                                   locRightFoot,
                                   locLeftLegUpper,
                                   locLeftLegLower,
                                   locLeftFoot,
                                   locGun);

    class JsonAttIdleSettings
    {
    public:
        float hipIdleAmount;
        float hipIdleSpeed;
        float idleCrouchFactor;
        float idleProneFactor;
        float adsIdleLerpStartTime;
        float adsIdleLerpTime;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttIdleSettings, hipIdleAmount, hipIdleSpeed, idleCrouchFactor, idleProneFactor, adsIdleLerpStartTime, adsIdleLerpTime);

    class JsonAttADSSettings
    {
    public:
        float adsSpread;
        float adsAimPitch;
        float adsTransInTime;
        float adsTransOutTime;
        int adsReloadTransTime;
        float adsCrosshairInFrac;
        float adsCrosshairOutFrac;
        float adsZoomFov;
        float adsZoomInFrac;
        float adsZoomOutFrac;
        float adsBobFactor;
        float adsViewBobMult;
        float adsViewErrorMin;
        float adsViewErrorMax;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttADSSettings,
                                   adsSpread,
                                   adsAimPitch,
                                   adsTransInTime,
                                   adsTransOutTime,
                                   adsReloadTransTime,
                                   adsCrosshairInFrac,
                                   adsCrosshairOutFrac,
                                   adsZoomFov,
                                   adsZoomInFrac,
                                   adsZoomOutFrac,
                                   adsBobFactor,
                                   adsViewBobMult,
                                   adsViewErrorMin,
                                   adsViewErrorMax);

    class JsonAttHipSpread
    {
    public:
        float hipSpreadStandMin;
        float hipSpreadDuckedMin;
        float hipSpreadProneMin;
        float hipSpreadMax;
        float hipSpreadDuckedMax;
        float hipSpreadProneMax;
        float hipSpreadFireAdd;
        float hipSpreadTurnAdd;
        float hipSpreadMoveAdd;
        float hipSpreadDecayRate;
        float hipSpreadDuckedDecay;
        float hipSpreadProneDecay;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttHipSpread,
                                   hipSpreadStandMin,
                                   hipSpreadDuckedMin,
                                   hipSpreadProneMin,
                                   hipSpreadMax,
                                   hipSpreadDuckedMax,
                                   hipSpreadProneMax,
                                   hipSpreadFireAdd,
                                   hipSpreadTurnAdd,
                                   hipSpreadMoveAdd,
                                   hipSpreadDecayRate,
                                   hipSpreadDuckedDecay,
                                   hipSpreadProneDecay);

    class JsonAttGunKick
    {
    public:
        int hipGunKickReducedKickBullets;
        float hipGunKickReducedKickPercent;
        float hipGunKickPitchMin;
        float hipGunKickPitchMax;
        float hipGunKickYawMin;
        float hipGunKickYawMax;
        float hipGunKickAccel;
        float hipGunKickSpeedMax;
        float hipGunKickSpeedDecay;
        float hipGunKickStaticDecay;
        int adsGunKickReducedKickBullets;
        float adsGunKickReducedKickPercent;
        float adsGunKickPitchMin;
        float adsGunKickPitchMax;
        float adsGunKickYawMin;
        float adsGunKickYawMax;
        float adsGunKickAccel;
        float adsGunKickSpeedMax;
        float adsGunKickSpeedDecay;
        float adsGunKickStaticDecay;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttGunKick,
                                   hipGunKickReducedKickBullets,
                                   hipGunKickReducedKickPercent,
                                   hipGunKickPitchMin,
                                   hipGunKickPitchMax,
                                   hipGunKickYawMin,
                                   hipGunKickYawMax,
                                   hipGunKickAccel,
                                   hipGunKickSpeedMax,
                                   hipGunKickSpeedDecay,
                                   hipGunKickStaticDecay,
                                   adsGunKickReducedKickBullets,
                                   adsGunKickReducedKickPercent,
                                   adsGunKickPitchMin,
                                   adsGunKickPitchMax,
                                   adsGunKickYawMin,
                                   adsGunKickYawMax,
                                   adsGunKickAccel,
                                   adsGunKickSpeedMax,
                                   adsGunKickSpeedDecay,
                                   adsGunKickStaticDecay);

    class JsonAttViewKick
    {
    public:
        float hipViewKickPitchMin;
        float hipViewKickPitchMax;
        float hipViewKickYawMin;
        float hipViewKickYawMax;
        float hipViewKickCenterSpeed;
        float adsViewKickPitchMin;
        float adsViewKickPitchMax;
        float adsViewKickYawMin;
        float adsViewKickYawMax;
        float adsViewKickCenterSpeed;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttViewKick,
                                   hipViewKickPitchMin,
                                   hipViewKickPitchMax,
                                   hipViewKickYawMin,
                                   hipViewKickYawMax,
                                   hipViewKickCenterSpeed,
                                   adsViewKickPitchMin,
                                   adsViewKickPitchMax,
                                   adsViewKickYawMin,
                                   adsViewKickYawMax,
                                   adsViewKickCenterSpeed);

    NLOHMANN_JSON_SERIALIZE_ENUM(weapOverlayReticle_t,
                                 {
                                     {WEAPOVERLAYRETICLE_NONE,      "none"     },
                                     {WEAPOVERLAYRETICLE_CROSSHAIR, "crosshair"},
    });

    class JsonAttADSOverlay
    {
    public:
        std::optional<std::string> shader;
        std::optional<std::string> shaderLowRes;
        std::optional<std::string> shaderEMP;
        std::optional<std::string> shaderEMPLowRes;
        weapOverlayReticle_t reticle;
        float width;
        float height;
        float widthSplitscreen;
        float heightSplitscreen;
        bool thermalScope;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(
        JsonAttADSOverlay, shader, shaderLowRes, shaderEMP, shaderEMPLowRes, reticle, width, height, widthSplitscreen, heightSplitscreen, thermalScope);

    NLOHMANN_JSON_SERIALIZE_ENUM(weaponIconRatioType_t,
                                 {
                                     {WEAPON_ICON_RATIO_1TO1, "1:1"},
                                     {WEAPON_ICON_RATIO_2TO1, "2:1"},
                                     {WEAPON_ICON_RATIO_4TO1, "4:1"},
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(ammoCounterClipType_t,
                                 {
                                     {AMMO_COUNTER_CLIP_NONE,          "none"         },
                                     {AMMO_COUNTER_CLIP_MAGAZINE,      "magazine"     },
                                     {AMMO_COUNTER_CLIP_SHORTMAGAZINE, "shortmagazine"},
                                     {AMMO_COUNTER_CLIP_SHOTGUN,       "shotgun"      },
                                     {AMMO_COUNTER_CLIP_ROCKET,        "rocket"       },
                                     {AMMO_COUNTER_CLIP_BELTFED,       "beltfed"      },
                                     {AMMO_COUNTER_CLIP_ALTWEAPON,     "altweapon"    },
    });

    class JsonAttUI
    {
    public:
        std::optional<std::string> dpadIcon;
        std::optional<std::string> ammoCounterIcon;
        weaponIconRatioType_t dpadIconRatio;
        weaponIconRatioType_t ammoCounterIconRatio;
        ammoCounterClipType_t ammoCounterClip;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttUI, dpadIcon, ammoCounterIcon, dpadIconRatio, ammoCounterIconRatio, ammoCounterClip);

    class JsonAttRumbles
    {
    public:
        std::optional<std::string> fireRumble;
        std::optional<std::string> meleeImpactRumble;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttRumbles, fireRumble, meleeImpactRumble);

    NLOHMANN_JSON_SERIALIZE_ENUM(weapProjExposion_t,
                                 {
                                     {WEAPPROJEXP_GRENADE,   "grenade"  },
                                     {WEAPPROJEXP_ROCKET,    "rocket"   },
                                     {WEAPPROJEXP_FLASHBANG, "flashbang"},
                                     {WEAPPROJEXP_NONE,      "none"     },
                                     {WEAPPROJEXP_DUD,       "dud"      },
                                     {WEAPPROJEXP_SMOKE,     "smoke"    },
                                     {WEAPPROJEXP_HEAVY,     "heavy"    },
    });

    class JsonAttProjectile
    {
    public:
        int explosionRadius;
        int explosionInnerDamage;
        int explosionOuterDamage;
        float damageConeAngle;
        int projectileSpeed;
        int projectileSpeedUp;
        int projectileActivateDist;
        float projectileLifetime;
        std::optional<std::string> projectileModel;
        weapProjExposion_t projExplosionType;
        std::optional<std::string> projExplosionEffect;
        bool projExplosionEffectForceNormalUp;
        std::optional<std::string> projExplosionSound;
        std::optional<std::string> projDudEffect;
        std::optional<std::string> projDudSound;
        bool projImpactExplode;
        float destabilizationRateTime;
        float destabilizationCurvatureMax;
        int destabilizeDistance;
        std::optional<std::string> projTrailEffect;
        int projIgnitionDelay;
        std::optional<std::string> projIgnitionEffect;
        std::optional<std::string> projIgnitionSound;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAttProjectile,
                                   explosionRadius,
                                   explosionInnerDamage,
                                   explosionOuterDamage,
                                   damageConeAngle,
                                   projectileSpeed,
                                   projectileSpeedUp,
                                   projectileActivateDist,
                                   projectileLifetime,
                                   projectileModel,
                                   projExplosionType,
                                   projExplosionEffect,
                                   projExplosionEffectForceNormalUp,
                                   projExplosionSound,
                                   projDudEffect,
                                   projDudSound,
                                   projImpactExplode,
                                   destabilizationRateTime,
                                   destabilizationCurvatureMax,
                                   destabilizeDistance,
                                   projTrailEffect,
                                   projIgnitionDelay,
                                   projIgnitionEffect,
                                   projIgnitionSound);

    NLOHMANN_JSON_SERIALIZE_ENUM(AttachmentType,
                                 {
                                     {ATTACHMENT_SCOPE,       "scope"      },
                                     {ATTACHMENT_UNDERBARREL, "underbarrel"},
                                     {ATTACHMENT_OTHER,       "other"      },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(weapType_t,
                                 {
                                     {WEAPTYPE_NONE,       "none"      },
                                     {WEAPTYPE_BULLET,     "bullet"    },
                                     {WEAPTYPE_GRENADE,    "grenade"   },
                                     {WEAPTYPE_PROJECTILE, "projectile"},
                                     {WEAPTYPE_RIOTSHIELD, "riotshield"},
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(weapClass_t,
                                 {
                                     {WEAPCLASS_RIFLE,          "rifle"         },
                                     {WEAPCLASS_SNIPER,         "sniper"        },
                                     {WEAPCLASS_MG,             "mg"            },
                                     {WEAPCLASS_SMG,            "smg"           },
                                     {WEAPCLASS_SPREAD,         "spread"        },
                                     {WEAPCLASS_PISTOL,         "pistol"        },
                                     {WEAPCLASS_GRENADE,        "grenade"       },
                                     {WEAPCLASS_ROCKETLAUNCHER, "rocketlauncher"},
                                     {WEAPCLASS_TURRET,         "turret"        },
                                     {WEAPCLASS_THROWINGKNIFE,  "throwingknife" },
                                     {WEAPCLASS_NON_PLAYER,     "nonPlayer"     },
                                     {WEAPCLASS_ITEM,           "item"          },
    });

    class JsonWeaponAttachment
    {
    public:
        std::string displayName;
        AttachmentType type;
        weapType_t weaponType;
        weapClass_t weapClass;
        std::vector<std::string> worldModels;
        std::vector<std::string> viewModels;
        std::vector<std::string> reticleViewModels;
        std::optional<JsonAttAmmoGeneral> ammoGeneral;
        std::optional<JsonAttSight> sight;
        std::optional<JsonAttReload> reload;
        std::optional<JsonAttAddOns> addOns;
        std::optional<JsonAttGeneral> general;
        std::optional<JsonAttAimAssist> aimAssist;
        std::optional<JsonAttAmmunition> ammunition;
        std::optional<JsonAttDamage> damage;
        std::optional<JsonAttLocationDamage> locationDamage;
        std::optional<JsonAttIdleSettings> idleSettings;
        std::optional<JsonAttADSSettings> adsSettings;
        std::optional<JsonAttADSSettings> adsSettingsMain;
        std::optional<JsonAttHipSpread> hipSpread;
        std::optional<JsonAttGunKick> gunKick;
        std::optional<JsonAttViewKick> viewKick;
        std::optional<JsonAttADSOverlay> adsOverlay;
        std::optional<JsonAttUI> ui;
        std::optional<JsonAttRumbles> rumbles;
        std::optional<JsonAttProjectile> projectile;
        float ammunitionScale;
        float damageScale;
        float damageScaleMin;
        float stateTimersScale;
        float fireTimersScale;
        float idleSettingsScale;
        float adsSettingsScale;
        float adsSettingsScaleMain;
        float hipSpreadScale;
        float gunKickScale;
        float viewKickScale;
        float viewCenterScale;
        int loadIndex;
        bool hideIronSightsWithThisAttachment;
        bool shareAmmoWithAlt;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonWeaponAttachment,
                                   displayName,
                                   type,
                                   weaponType,
                                   weapClass,
                                   worldModels,
                                   viewModels,
                                   reticleViewModels,
                                   ammoGeneral,
                                   sight,
                                   reload,
                                   addOns,
                                   general,
                                   aimAssist,
                                   ammunition,
                                   damage,
                                   locationDamage,
                                   idleSettings,
                                   adsSettings,
                                   adsSettingsMain,
                                   hipSpread,
                                   gunKick,
                                   viewKick,
                                   adsOverlay,
                                   ui,
                                   rumbles,
                                   projectile,
                                   ammunitionScale,
                                   damageScale,
                                   damageScaleMin,
                                   stateTimersScale,
                                   fireTimersScale,
                                   idleSettingsScale,
                                   adsSettingsScale,
                                   adsSettingsScaleMain,
                                   hipSpreadScale,
                                   gunKickScale,
                                   viewKickScale,
                                   viewCenterScale,
                                   loadIndex,
                                   hideIronSightsWithThisAttachment,
                                   shareAmmoWithAlt);
} // namespace IW5
