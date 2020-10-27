#include "AssetDumperVehicle.h"

#include <cassert>
#include <sstream>

#include "Game/IW4/InfoStringIW4.h"

using namespace IW4;

cspField_t AssetDumperVehicle::vehicle_fields[]
{
    { "type", offsetof(VehicleDef, type), VFT_TYPE },
    { "useHintString", offsetof(VehicleDef, useHintString), CSPFT_STRING },
    { "health", offsetof(VehicleDef, health), CSPFT_INT },
    { "quadBarrel", offsetof(VehicleDef, quadBarrel), CSPFT_QBOOLEAN },
    { "texureScrollScale", offsetof(VehicleDef, texScrollScale), CSPFT_FLOAT },
    { "topSpeed", offsetof(VehicleDef, topSpeed), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "accel", offsetof(VehicleDef, accel), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "rotRate", offsetof(VehicleDef, rotRate), CSPFT_FLOAT },
    { "rotAccel", offsetof(VehicleDef, rotAccel), CSPFT_FLOAT },
    { "maxBodyPitch", offsetof(VehicleDef, maxBodyPitch), CSPFT_FLOAT },
    { "maxBodyRoll", offsetof(VehicleDef, maxBodyRoll), CSPFT_FLOAT },
    { "fakeBodyAccelPitch", offsetof(VehicleDef, fakeBodyAccelPitch), CSPFT_FLOAT },
    { "fakeBodyAccelRoll", offsetof(VehicleDef, fakeBodyAccelRoll), CSPFT_FLOAT },
    { "fakeBodyVelPitch", offsetof(VehicleDef, fakeBodyVelPitch), CSPFT_FLOAT },
    { "fakeBodyVelRoll", offsetof(VehicleDef, fakeBodyVelRoll), CSPFT_FLOAT },
    { "fakeBodySideVelPitch", offsetof(VehicleDef, fakeBodySideVelPitch), CSPFT_FLOAT },
    { "fakeBodyPitchStrength", offsetof(VehicleDef, fakeBodyPitchStrength), CSPFT_FLOAT },
    { "fakeBodyRollStrength", offsetof(VehicleDef, fakeBodyRollStrength), CSPFT_FLOAT },
    { "fakeBodyPitchDampening", offsetof(VehicleDef, fakeBodyPitchDampening), CSPFT_FLOAT },
    { "fakeBodyRollDampening", offsetof(VehicleDef, fakeBodyRollDampening), CSPFT_FLOAT },
    { "fakeBodyBoatRockingAmplitude", offsetof(VehicleDef, fakeBodyBoatRockingAmplitude), CSPFT_FLOAT },
    { "fakeBodyBoatRockingPeriod", offsetof(VehicleDef, fakeBodyBoatRockingPeriod), CSPFT_FLOAT },
    { "fakeBodyBoatRockingRotationPeriod", offsetof(VehicleDef, fakeBodyBoatRockingRotationPeriod), CSPFT_FLOAT },
    { "fakeBodyBoatRockingFadeoutSpeed", offsetof(VehicleDef, fakeBodyBoatRockingFadeoutSpeed), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "boatBouncingMinForce", offsetof(VehicleDef, boatBouncingMinForce), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "boatBouncingMaxForce", offsetof(VehicleDef, boatBouncingMaxForce), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "boatBouncingRate", offsetof(VehicleDef, boatBouncingRate), CSPFT_FLOAT },
    { "boatBouncingFadeinSpeed", offsetof(VehicleDef, boatBouncingFadeinSpeed), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "boatBouncingFadeoutSteeringAngle", offsetof(VehicleDef, boatBouncingFadeoutSteeringAngle), CSPFT_FLOAT },
    { "collisionDamage", offsetof(VehicleDef, collisionDamage), CSPFT_FLOAT },
    { "collisionSpeed", offsetof(VehicleDef, collisionSpeed), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "killcamZDist", offsetof(VehicleDef, killcamOffset[0]), CSPFT_FLOAT },
    { "killcamBackDist", offsetof(VehicleDef, killcamOffset[1]), CSPFT_FLOAT },
    { "killcamUpDist", offsetof(VehicleDef, killcamOffset[2]), CSPFT_FLOAT },
    { "playerProtected", offsetof(VehicleDef, playerProtected), CSPFT_QBOOLEAN },
    { "bulletDamage", offsetof(VehicleDef, bulletDamage), CSPFT_QBOOLEAN },
    { "armorPiercingDamage", offsetof(VehicleDef, armorPiercingDamage), CSPFT_QBOOLEAN },
    { "grenadeDamage", offsetof(VehicleDef, grenadeDamage), CSPFT_QBOOLEAN },
    { "projectileDamage", offsetof(VehicleDef, projectileDamage), CSPFT_QBOOLEAN },
    { "projectileSplashDamage", offsetof(VehicleDef, projectileSplashDamage), CSPFT_QBOOLEAN },
    { "heavyExplosiveDamage", offsetof(VehicleDef, heavyExplosiveDamage), CSPFT_QBOOLEAN },
    { "physicsEnabled", offsetof(VehicleDef, vehPhysDef.physicsEnabled), CSPFT_QBOOLEAN },
    { "physicsPreset", offsetof(VehicleDef, vehPhysDef.physPresetName), CSPFT_STRING },
    { "accelerationGraph", offsetof(VehicleDef, vehPhysDef.accelGraphName), CSPFT_STRING },
    { "steeringAxle", offsetof(VehicleDef, vehPhysDef.steeringAxle), VFT_AXLE_STEERING },
    { "powerAxle", offsetof(VehicleDef, vehPhysDef.powerAxle), VFT_AXLE_POWER },
    { "brakingAxle", offsetof(VehicleDef, vehPhysDef.brakingAxle), VFT_AXLE_BRAKING },
    { "reverseSpeed", offsetof(VehicleDef, vehPhysDef.reverseSpeed), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "maxVelocity", offsetof(VehicleDef, vehPhysDef.maxVelocity), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "maxPitch", offsetof(VehicleDef, vehPhysDef.maxPitch), CSPFT_FLOAT },
    { "maxRoll", offsetof(VehicleDef, vehPhysDef.maxRoll), CSPFT_FLOAT },
    { "suspensionTravelRear", offsetof(VehicleDef, vehPhysDef.suspensionTravelRear), CSPFT_FLOAT },
    { "suspensionStrengthFront", offsetof(VehicleDef, vehPhysDef.suspensionStrengthFront), CSPFT_FLOAT },
    { "suspensionDampingFront", offsetof(VehicleDef, vehPhysDef.suspensionDampingFront), CSPFT_FLOAT },
    { "suspensionStrengthRear", offsetof(VehicleDef, vehPhysDef.suspensionStrengthRear), CSPFT_FLOAT },
    { "suspensionDampingRear", offsetof(VehicleDef, vehPhysDef.suspensionDampingRear), CSPFT_FLOAT },
    { "frictionBraking", offsetof(VehicleDef, vehPhysDef.frictionBraking), CSPFT_FLOAT },
    { "frictionCoasting", offsetof(VehicleDef, vehPhysDef.frictionCoasting), CSPFT_FLOAT },
    { "frictionTopSpeed", offsetof(VehicleDef, vehPhysDef.frictionTopSpeed), CSPFT_FLOAT },
    { "frictionSide", offsetof(VehicleDef, vehPhysDef.frictionSide), CSPFT_FLOAT },
    { "frictionSideRear", offsetof(VehicleDef, vehPhysDef.frictionSideRear), CSPFT_FLOAT },
    { "velocityDependentSlip", offsetof(VehicleDef, vehPhysDef.velocityDependentSlip), CSPFT_FLOAT },
    { "rollStability", offsetof(VehicleDef, vehPhysDef.rollStability), CSPFT_FLOAT },
    { "rollResistance", offsetof(VehicleDef, vehPhysDef.rollResistance), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "pitchResistance", offsetof(VehicleDef, vehPhysDef.pitchResistance), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "yawResistance", offsetof(VehicleDef, vehPhysDef.yawResistance), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "uprightStrengthPitch", offsetof(VehicleDef, vehPhysDef.uprightStrengthPitch), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "uprightStrengthRoll", offsetof(VehicleDef, vehPhysDef.uprightStrengthRoll), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "targetAirPitch", offsetof(VehicleDef, vehPhysDef.targetAirPitch), CSPFT_FLOAT },
    { "airYawTorque", offsetof(VehicleDef, vehPhysDef.airYawTorque), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "airPitchTorque", offsetof(VehicleDef, vehPhysDef.airPitchTorque), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "minimumMomentumForCollision", offsetof(VehicleDef, vehPhysDef.minimumMomentumForCollision), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "collisionLaunchForceScale", offsetof(VehicleDef, vehPhysDef.collisionLaunchForceScale), CSPFT_FLOAT },
    { "wreckedMassScale", offsetof(VehicleDef, vehPhysDef.wreckedMassScale), CSPFT_FLOAT },
    { "wreckedBodyFriction", offsetof(VehicleDef, vehPhysDef.wreckedBodyFriction), CSPFT_FLOAT },
    { "minimumJoltForNotify", offsetof(VehicleDef, vehPhysDef.minimumJoltForNotify), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "slipThresholdFront", offsetof(VehicleDef, vehPhysDef.slipThresholdFront), CSPFT_FLOAT },
    { "slipThresholdRear", offsetof(VehicleDef, vehPhysDef.slipThresholdRear), CSPFT_FLOAT },
    { "slipFricScaleFront", offsetof(VehicleDef, vehPhysDef.slipFricScaleFront), CSPFT_FLOAT },
    { "slipFricScaleRear", offsetof(VehicleDef, vehPhysDef.slipFricScaleRear), CSPFT_FLOAT },
    { "slipFricRateFront", offsetof(VehicleDef, vehPhysDef.slipFricRateFront), CSPFT_FLOAT },
    { "slipFricRateRear", offsetof(VehicleDef, vehPhysDef.slipFricRateRear), CSPFT_FLOAT },
    { "slipYawTorque", offsetof(VehicleDef, vehPhysDef.slipYawTorque), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "boostDuration", offsetof(VehicleDef, boostDuration), CSPFT_FLOAT },
    { "boostRechargeTime", offsetof(VehicleDef, boostRechargeTime), CSPFT_FLOAT },
    { "boostAcceleration", offsetof(VehicleDef, boostAcceleration), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "suspensionTravel", offsetof(VehicleDef, suspensionTravel), CSPFT_FLOAT },
    { "maxSteeringAngle", offsetof(VehicleDef, maxSteeringAngle), CSPFT_FLOAT },
    { "steeringLerp", offsetof(VehicleDef, steeringLerp), CSPFT_FLOAT },
    { "minSteeringScale", offsetof(VehicleDef, minSteeringScale), CSPFT_FLOAT },
    { "minSteeringSpeed", offsetof(VehicleDef, minSteeringSpeed), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "camLookEnabled", offsetof(VehicleDef, camLookEnabled), CSPFT_QBOOLEAN },
    { "camLerp", offsetof(VehicleDef, camLerp), CSPFT_FLOAT },
    { "camPitchInfluence", offsetof(VehicleDef, camPitchInfluence), CSPFT_FLOAT },
    { "camRollInfluence", offsetof(VehicleDef, camRollInfluence), CSPFT_FLOAT },
    { "camFovIncrease", offsetof(VehicleDef, camFovIncrease), CSPFT_FLOAT },
    { "camFovOffset", offsetof(VehicleDef, camFovOffset), CSPFT_FLOAT },
    { "camFovSpeed", offsetof(VehicleDef, camFovSpeed), CSPFT_FLOAT },
    { "turretWeaponName", offsetof(VehicleDef, turretWeaponName), CSPFT_STRING },
    { "turretHorizSpanLeft", offsetof(VehicleDef, turretHorizSpanLeft), CSPFT_FLOAT },
    { "turretHorizSpanRight", offsetof(VehicleDef, turretHorizSpanRight), CSPFT_FLOAT },
    { "turretVertSpanUp", offsetof(VehicleDef, turretVertSpanUp), CSPFT_FLOAT },
    { "turretVertSpanDown", offsetof(VehicleDef, turretVertSpanDown), CSPFT_FLOAT },
    { "turretRotRate", offsetof(VehicleDef, turretRotRate), CSPFT_FLOAT },
    { "turretSpinSnd", offsetof(VehicleDef, turretSpinSnd), CSPFT_SOUND },
    { "turretStopSnd", offsetof(VehicleDef, turretStopSnd), CSPFT_SOUND },
    { "trophyEnabled", offsetof(VehicleDef, trophyEnabled), CSPFT_QBOOLEAN },
    { "trophyRadius", offsetof(VehicleDef, trophyRadius), CSPFT_FLOAT },
    { "trophyInactiveRadius", offsetof(VehicleDef, trophyInactiveRadius), CSPFT_FLOAT },
    { "trophyAmmoCount", offsetof(VehicleDef, trophyAmmoCount), CSPFT_INT },
    { "trophyReloadTime", offsetof(VehicleDef, trophyReloadTime), CSPFT_FLOAT },
    { "trophyTags", offsetof(VehicleDef, trophyTags), VFT_TROPHY_TAGS },
    { "compassFriendlyIcon", offsetof(VehicleDef, compassFriendlyIcon), CSPFT_MATERIAL },
    { "compassEnemyIcon", offsetof(VehicleDef, compassEnemyIcon), CSPFT_MATERIAL },
    { "compassIconWidth", offsetof(VehicleDef, compassIconWidth), CSPFT_INT },
    { "compassIconHeight", offsetof(VehicleDef, compassIconHeight), CSPFT_INT },
    { "lowIdleSnd", offsetof(VehicleDef, idleLowSnd), CSPFT_SOUND },
    { "highIdleSnd", offsetof(VehicleDef, idleHighSnd), CSPFT_SOUND },
    { "lowEngineSnd", offsetof(VehicleDef, engineLowSnd), CSPFT_SOUND },
    { "highEngineSnd", offsetof(VehicleDef, engineHighSnd), CSPFT_SOUND },
    { "engineSndSpeed", offsetof(VehicleDef, engineSndSpeed), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "engineStartUpSnd", offsetof(VehicleDef, engineStartUpSnd), CSPFT_SOUND },
    { "engineStartUpLength", offsetof(VehicleDef, engineStartUpLength), CSPFT_MILLISECONDS },
    { "engineShutdownSnd", offsetof(VehicleDef, engineShutdownSnd), CSPFT_SOUND },
    { "engineIdleSnd", offsetof(VehicleDef, engineIdleSnd), CSPFT_SOUND },
    { "engineSustainSnd", offsetof(VehicleDef, engineSustainSnd), CSPFT_SOUND },
    { "engineRampUpSnd", offsetof(VehicleDef, engineRampUpSnd), CSPFT_SOUND },
    { "engineRampUpLength", offsetof(VehicleDef, engineRampUpLength), CSPFT_MILLISECONDS },
    { "engineRampDownSnd", offsetof(VehicleDef, engineRampDownSnd), CSPFT_SOUND },
    { "engineRampDownLength", offsetof(VehicleDef, engineRampDownLength), CSPFT_MILLISECONDS },
    { "suspensionSoftSnd", offsetof(VehicleDef, suspensionSoftSnd), CSPFT_SOUND },
    { "suspensionSoftCompression", offsetof(VehicleDef, suspensionSoftCompression), CSPFT_FLOAT },
    { "suspensionHardSnd", offsetof(VehicleDef, suspensionHardSnd), CSPFT_SOUND },
    { "suspensionHardCompression", offsetof(VehicleDef, suspensionHardCompression), CSPFT_FLOAT },
    { "collisionSnd", offsetof(VehicleDef, collisionSnd), CSPFT_SOUND },
    { "collisionBlendSpeed", offsetof(VehicleDef, collisionBlendSpeed), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "speedSnd", offsetof(VehicleDef, speedSnd), CSPFT_SOUND },
    { "speedSndBlendSpeed", offsetof(VehicleDef, speedSndBlendSpeed), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "surfaceSndPrefix", offsetof(VehicleDef, surfaceSndPrefix), CSPFT_STRING },
    { "surfaceSndBlendSpeed", offsetof(VehicleDef, surfaceSndBlendSpeed), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "slideVolume", offsetof(VehicleDef, slideVolume), CSPFT_FLOAT },
    { "slideBlendSpeed", offsetof(VehicleDef, slideBlendSpeed), CSPFT_MPH_TO_INCHES_PER_SEC },
    { "inAirPitch", offsetof(VehicleDef, inAirPitch), CSPFT_FLOAT },
};

namespace IW4
{
    const char* s_vehicleClassNames[]
    {
        "4 wheel",
        "tank",
        "plane",
        "boat",
        "artillery",
        "helicopter",
        "snowmobile",
    };

    const char* s_vehicleAxleTypeNames[]
    {
        "front",
        "rear",
        "all",
    };

    class InfoStringFromVehicleConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<VehicleFieldType>(field.iFieldType))
            {
            case VFT_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, s_vehicleClassNames, _countof(s_vehicleClassNames));
                break;

            case VFT_AXLE_STEERING:
            case VFT_AXLE_POWER:
            case VFT_AXLE_BRAKING:
                FillFromEnumInt(std::string(field.szName), field.iOffset, s_vehicleAxleTypeNames, _countof(s_vehicleAxleTypeNames));
                break;

            case VFT_TROPHY_TAGS:
            {
                const auto* trophyTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                std::stringstream ss;
                bool first = true;

                for (auto i = 0u; i < _countof(VehicleDef::trophyTags); i++)
                {
                    const auto& str = m_get_scr_string(trophyTags[i]);
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

            case VFT_NUM:
            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromVehicleConverter(const VehicleDef* structure, const cspField_t* fields, const size_t fieldCount, std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
}

bool AssetDumperVehicle::ShouldDump(XAssetInfo<VehicleDef>* asset)
{
    return true;
}

std::string AssetDumperVehicle::GetFileNameForAsset(Zone* zone, XAssetInfo<VehicleDef>* asset)
{
    return "vehicles/" + asset->m_name;
}

void AssetDumperVehicle::DumpAsset(Zone* zone, XAssetInfo<VehicleDef>* asset, FileAPI::File* out)
{
    InfoStringFromVehicleConverter converter(asset->Asset(), vehicle_fields, _countof(vehicle_fields), [asset](const scr_string_t scrStr) -> std::string
        {
            assert(scrStr < asset->m_zone->m_script_strings.size());
            if (scrStr >= asset->m_zone->m_script_strings.size())
                return "";

            return asset->m_zone->m_script_strings[scrStr];
        });

    const auto infoString = converter.Convert();
    const auto stringValue = infoString.ToString("VEHICLEFILE");
    out->Write(stringValue.c_str(), 1, stringValue.length());
}