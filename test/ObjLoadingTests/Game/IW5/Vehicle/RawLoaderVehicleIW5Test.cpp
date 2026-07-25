#include "Game/IW5/Vehicle/RawLoaderVehicleIW5.h"

#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace IW5;
using namespace Catch;
using namespace std::string_literals;

namespace
{
    XAssetInfo<PhysPreset>* GivenTestPhysPreset(const std::string& name, Zone& zone)
    {
        auto& memory = zone.Memory();
        auto* physPreset = memory.Alloc<PhysPreset>();
        physPreset->name = memory.Dup(name.c_str());
        return zone.m_pools.AddAsset<AssetPhysPreset>(std::make_unique<XAssetInfo<PhysPreset>>(ASSET_TYPE_PHYSPRESET, name, physPreset));
    }

    XAssetInfo<WeaponCompleteDef>* GivenTestWeapon(const std::string& name, Zone& zone)
    {
        auto& memory = zone.Memory();
        auto* weapon = memory.Alloc<WeaponCompleteDef>();
        weapon->szInternalName = memory.Dup(name.c_str());
        return zone.m_pools.AddAsset<AssetWeapon>(std::make_unique<XAssetInfo<WeaponCompleteDef>>(ASSET_TYPE_WEAPON, name, weapon));
    }

    IndirectAssetReference GivenTestSound(const std::string& name, Zone& zone)
    {
        auto& memory = zone.Memory();
        auto* sndAliasList = memory.Alloc<snd_alias_list_t>();
        sndAliasList->aliasName = memory.Dup(name.c_str());

        zone.m_pools.AddAsset<AssetSound>(std::make_unique<XAssetInfo<snd_alias_list_t>>(ASSET_TYPE_SOUND, name, sndAliasList));

        return IndirectAssetReference(ASSET_TYPE_SOUND, name);
    }

    XAssetInfo<Material>* GivenTestMaterial(const std::string& name, Zone& zone)
    {
        auto& memory = zone.Memory();
        auto* material = memory.Alloc<Material>();
        material->info.name = memory.Dup(name.c_str());
        return zone.m_pools.AddAsset<AssetMaterial>(std::make_unique<XAssetInfo<Material>>(ASSET_TYPE_MATERIAL, name, material));
    }

    XAssetInfo<FxEffectDef>* GivenTestFx(const std::string& name, Zone& zone)
    {
        auto& memory = zone.Memory();
        auto* fx = memory.Alloc<FxEffectDef>();
        fx->name = memory.Dup(name.c_str());
        return zone.m_pools.AddAsset<AssetFx>(std::make_unique<XAssetInfo<FxEffectDef>>(ASSET_TYPE_FX, name, fx));
    }

    TEST_CASE("Vehicle loader restores stock raw fields (IW5)", "[iw5][vehicle][system]")
    {
        constexpr auto RAW_VEHICLE =
            R"(VEHICLEFILE\type\4 wheel\useHintString\\health\100\quadBarrel\0\textureScrollScale\5.5\topSpeed\60.000000\accel\12.550000)"
            R"(\rotRate\60\rotAccel\100\maxBodyPitch\7\maxBodyRoll\7\fakeBodyStabilizer\1\fakeBodyStabilizer\1\fakeBodyAccelPitch\0.2)"
            R"(\fakeBodyAccelRoll\0.2\fakeBodyVelPitch\0\fakeBodyVelRoll\0\fakeBodySideVelPitch\0\fakeBodyPitchStrength\30)"
            R"(\fakeBodyRollStrength\30\fakeBodyPitchDampening\3\fakeBodyRollDampening\3)"
            R"(\fakeBodyBoatRockingAmplitude\5\fakeBodyBoatRockingPeriod\3\fakeBodyBoatRockingRotationPeriod\240)"
            R"(\fakeBodyBoatRockingFadeoutSpeed\15.000000\boatBouncingMinForce\37.000000\boatBouncingMaxForce\37.000000\boatBouncingRate\1.5)"
            R"(\boatBouncingFadeinSpeed\40.000000\boatBouncingFadeoutSteeringAngle\12\collisionDamage\200\collisionSpeed\15.000000)"
            R"(\killcamZDist\50\killcamBackDist\1000\killcamUpDist\0\playerProtected\1\bulletDamage\1\armorPiercingDamage\1\grenadeDamage\1)"
            R"(\projectileDamage\1\projectileSplashDamage\1\heavyExplosiveDamage\1\physicsEnabled\1\physicsPreset\vehicle_tank\accelerationGraph\tank_accel.graph)"
            R"(\steeringAxle\all\powerAxle\all\brakingAxle\all\reverseSpeed\20.000000\maxVelocity\100.000000\maxPitch\60\maxRoll\60)"
            R"(\suspensionTravelRear\0\suspensionStrengthFront\40\suspensionDampingFront\2\suspensionStrengthRear\40\suspensionDampingRear\2)"
            R"(\frictionBraking\5\frictionCoasting\1\frictionTopSpeed\1\frictionSide\2\frictionSideRear\2\velocityDependentSlip\0.0001)"
            R"(\rollStability\0.8\rollResistance\350.000000\pitchResistance\18.000000\yawResistance\350.000000\uprightStrengthPitch\10.000000)"
            R"(\uprightStrengthRoll\35.000000\targetAirPitch\0\airYawTorque\1.400000\airPitchTorque\0.350000\minimumMomentumForCollision\300.000000)"
            R"(\collisionLaunchForceScale\1.75\wreckedMassScale\0.1\wreckedBodyFriction\0.6\minimumJoltForNotify\2000.000000)"
            R"(\slipThresholdFront\800\slipThresholdRear\400\slipFricScaleFront\0.2\slipFricScaleRear\0.1\slipFricRateFront\1\slipFricRateRear\1)"
            R"(\slipYawTorque\2.800000\boostDuration\0\boostRechargeTime\0\boostAcceleration\0.000000\suspensionTravel\12\maxSteeringAngle\45)"
            R"(\steeringLerp\5\minSteeringScale\1\minSteeringSpeed\0.000000)"
            R"(\vehHelicopterBoundsRadius\1.0\vehHelicopterMaxSpeed\2.0\vehHelicopterMaxSpeedVertical\3.0\vehHelicopterMaxAccel\4.0)"
            R"(\vehHelicopterMaxAccelVertical\5.0\vehHelicopterDecelerationFwd\6.0\vehHelicopterDecelerationSide\7.0\vehHelicopterDecelerationUp\8.0)"
            R"(\vehHelicopterMaxYawRate\9.0\vehHelicopterMaxYawAccel\10.0\vehHelicopterTiltFromVelocity\11.0\vehHelicopterTiltFromControllerAxes\12.0)"
            R"(\vehHelicopterTiltFromAcceleration\13.0\vehHelicopterTiltFromDeceleration\14.0)"
            R"(\vehHelicopterTiltFromFwdAndYaw_VelAtMaxTilt\15.0\vehHelicopterTiltFromFwdAndYaw\16.0\vehHelicopterTiltMomentum\17.0)"
            R"(\vehHelicopterTiltSpeed\18.0\vehHelicopterMaxPitch\19.0\vehHelicopterMaxRoll\20.0\vehHelicopterHoverSpeedThreshold\21.0)"
            R"(\vehHelicopterJitterJerkyness\22.0\vehHelicopterLookaheadTime\23.0\vehHelicopterSoftCollisions\1)"
            R"(\camLookEnabled\0\camRelativeControl\1\camLerp\8\camHeight\9.0\camRadius\10.0\camPitchInfluence\1\camYawInfluence\2\camRollInfluence\0.6)"
            R"(\camFovIncrease\20\camFovOffset\4\camFovSpeed\65\camReturnSpeed\1\camReturnLerp\2\vehCam_UseGDT\1\vehCam_anglesPitch\4\vehCam_anglesYaw\5)"
            R"(\vehCam_anglesRoll\6\vehCam_offsetX\7\vehCam_offsetY\8\vehCam_offsetZ\9\vehCam_radius\10\vehCam_speedInfluence\11\vehCam_pitchTurnRate\12)"
            R"(\vehCam_pitchClamp\13\vehCam_yawTurnRate\14\vehCam_yawClamp\15\vehCam_zOffsetMode\world\vehCam_anglesPitch3P\17\vehCam_anglesYaw3P\18)"
            R"(\vehCam_anglesRoll3P\19\vehCam_offsetX3P\20\vehCam_offsetY3P\21\vehCam_offsetZ3P\22\vehCam_radius3P\23\vehCam_speedInfluence3P\24)"
            R"(\vehCam_pitchTurnRate3P\25\vehCam_pitchClamp3P\26\vehCam_yawTurnRate3P\27\vehCam_yawClamp3P\28\vehCam_zOffsetMode3P\vehicle\)"
            R"(turretWeaponName\sp/bradley_turret\turretHorizSpanLeft\180\turretHorizSpanRight\180)"
            R"(\turretVertSpanUp\30\turretVertSpanDown\15\turretHorizResistLeft\1\turretHorizResistRight\2\turretVertResistUp\3\turretVertResistDown\4)"
            R"(\turretRotRate\30\turretSpinSnd\bradley_turret_spin\turretStopSnd\bradley_turret_stop)"
            R"(\trophyEnabled\1\trophyRadius\450\trophyInactiveRadius\400\trophyAmmoCount\30\trophyReloadTime\1\trophyTags\tag_trophy_01_fx
tag_trophy_02_fx
tag_trophy_03_fx
tag_trophy_04_fx\trophyExplodeFx\testfx1\trophyFlashFx\testfx2)"
            R"(\compassFriendlyIcon\hud_minimap_harrier_green\compassEnemyIcon\hud_minimap_harrier_red\compassFriendlyAltIcon\hud_minimap_harrier_alt_green)"
            R"(\compassEnemyAltIcon\hud_minimap_harrier_alt_red\compassIconWidth\26\compassIconHeight\26)"
            R"(\lowIdleSnd\veh_ambulance_idle_low\highIdleSnd\veh_ambulance_idle_high\lowEngineSnd\veh_ambulance_engine_low)"
            R"(\highEngineSnd\veh_ambulance_engine_high\engineSndSpeed\10.000000\audioOriginTag\foo1\lowIdleSndAlt\foo2\highIdleSndAlt\foo3\)"
            R"(lowEngineSndAlt\foo4\highEngineSndAlt\foo5\engineSndSpeedAlt\3\audioOriginTagAlt\foo6\turretSpinSndAlt\foo7\turretStopSndAlt\foo8)"
            R"(\engineStartUpSnd\\engineStartUpLength\0\engineShutdownSnd\)"
            R"(\engineIdleSnd\snowmobile_2_idle\engineSustainSnd\snowmobile_2_sustain\engineRampUpSnd\snowmobile_2_rampup\engineRampUpLength\3.263)"
            R"(\engineRampDownSnd\snowmobile_2_rampdown\engineRampDownLength\5.262\suspensionSoftSnd\snowmobile_suspension_npc)"
            R"(\suspensionSoftCompression\0.3\suspensionHardSnd\snowmobile_suspension_heavy_npc\suspensionHardCompression\0.8\collisionSnd\)"
            R"(\collisionBlendSpeed\20.000000\collisionSnd\snowmobile_collision\collisionBlendSpeed\20.000000\speedSnd\snowmobile_wind_loop)"
            R"(\speedSndBlendSpeed\60.000000\surfaceSndPrefix\snowmobile_surface\surfaceSndBlendSpeed\10.000000\slideVolume\0.8)"
            R"(\slideBlendSpeed\20.000000\inAirPitch\1.3)";

        MockSearchPath loadingSearchPath;
        loadingSearchPath.AddFileData("vehicles/test_vehicle", RAW_VEHICLE);

        Zone zone("LoadingZone", 0, GameId::IW5, GamePlatform::PC);

        auto* physPresetInfo = GivenTestPhysPreset("vehicle_tank", zone);
        auto* weaponInfo = GivenTestWeapon("sp/bradley_turret", zone);
        auto* material0 = GivenTestMaterial("hud_minimap_harrier_green", zone);
        auto* material1 = GivenTestMaterial("hud_minimap_harrier_red", zone);
        auto* material2 = GivenTestMaterial("hud_minimap_harrier_alt_green", zone);
        auto* material3 = GivenTestMaterial("hud_minimap_harrier_alt_red", zone);
        auto* fx0 = GivenTestFx("testfx1", zone);
        auto* fx1 = GivenTestFx("testfx2", zone);
        auto sound0 = GivenTestSound("bradley_turret_spin", zone);
        auto sound1 = GivenTestSound("bradley_turret_stop", zone);
        auto sound2 = GivenTestSound("veh_ambulance_idle_low", zone);
        auto sound3 = GivenTestSound("veh_ambulance_idle_high", zone);
        auto sound4 = GivenTestSound("veh_ambulance_engine_low", zone);
        auto sound5 = GivenTestSound("veh_ambulance_engine_high", zone);
        auto sound6 = GivenTestSound("snowmobile_2_idle", zone);
        auto sound7 = GivenTestSound("snowmobile_2_sustain", zone);
        auto sound8 = GivenTestSound("snowmobile_2_rampup", zone);
        auto sound9 = GivenTestSound("snowmobile_2_rampdown", zone);
        auto sound10 = GivenTestSound("snowmobile_suspension_npc", zone);
        auto sound11 = GivenTestSound("snowmobile_suspension_heavy_npc", zone);
        auto sound12 = GivenTestSound("snowmobile_collision", zone);
        auto sound13 = GivenTestSound("snowmobile_wind_loop", zone);
        auto sound14 = GivenTestSound("snowmobile_surface_default", zone);

        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext loadingContext(zone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = vehicle::CreateRawLoaderIW5(zone.Memory(), loadingSearchPath, zone);
        const auto result = loader->CreateAsset("test_vehicle", loadingContext);

        REQUIRE(result.HasBeenSuccessful());
        const auto* loadedAssetInfo = reinterpret_cast<XAssetInfo<VehicleDef>*>(result.GetAssetInfo());
        const auto* loadedVehicle = loadedAssetInfo->Asset();

        REQUIRE(std::string(loadedVehicle->name) == "test_vehicle");
        REQUIRE(loadedVehicle->type == VEH_WHEELS_4);
        REQUIRE(loadedVehicle->health == 100);
        REQUIRE(loadedVehicle->quadBarrel == 0);
        REQUIRE(loadedVehicle->texScrollScale == Approx(5.5f));
        REQUIRE(loadedVehicle->topSpeed == Approx(1056.0f));
        REQUIRE(loadedVehicle->accel == Approx(220.88f));
        REQUIRE(loadedVehicle->rotRate == Approx(60.0f));
        REQUIRE(loadedVehicle->rotAccel == Approx(100.0f));
        REQUIRE(loadedVehicle->maxBodyPitch == Approx(7.0f));
        REQUIRE(loadedVehicle->maxBodyRoll == Approx(7.0f));
        REQUIRE(loadedVehicle->fakeBodyStabilizer == 1);
        REQUIRE(loadedVehicle->fakeBodyAccelPitch == Approx(0.2f));
        REQUIRE(loadedVehicle->fakeBodyAccelRoll == Approx(0.2f));
        REQUIRE(loadedVehicle->fakeBodyVelPitch == Approx(0.0f));
        REQUIRE(loadedVehicle->fakeBodyVelRoll == Approx(0.0f));
        REQUIRE(loadedVehicle->fakeBodySideVelPitch == Approx(0.0f));
        REQUIRE(loadedVehicle->fakeBodyPitchStrength == Approx(30.0f));
        REQUIRE(loadedVehicle->fakeBodyRollStrength == Approx(30.0f));
        REQUIRE(loadedVehicle->fakeBodyPitchDampening == Approx(3.0f));
        REQUIRE(loadedVehicle->fakeBodyRollDampening == Approx(3.0f));
        REQUIRE(loadedVehicle->fakeBodyBoatRockingAmplitude == Approx(5.0f));
        REQUIRE(loadedVehicle->fakeBodyBoatRockingPeriod == Approx(3.0f));
        REQUIRE(loadedVehicle->fakeBodyBoatRockingRotationPeriod == Approx(240.0f));
        REQUIRE(loadedVehicle->fakeBodyBoatRockingFadeoutSpeed == Approx(264.0f));
        REQUIRE(loadedVehicle->boatBouncingMinForce == Approx(651.2f));
        REQUIRE(loadedVehicle->boatBouncingMaxForce == Approx(651.2f));
        REQUIRE(loadedVehicle->boatBouncingRate == Approx(1.5f));
        REQUIRE(loadedVehicle->boatBouncingFadeinSpeed == Approx(704.0f));
        REQUIRE(loadedVehicle->boatBouncingFadeoutSteeringAngle == Approx(12.0f));
        REQUIRE(loadedVehicle->collisionDamage == Approx(200.0f));
        REQUIRE(loadedVehicle->collisionSpeed == Approx(264.0f));
        REQUIRE(loadedVehicle->killcamOffset[0] == Approx(50.0f));
        REQUIRE(loadedVehicle->killcamOffset[1] == Approx(1000.0f));
        REQUIRE(loadedVehicle->killcamOffset[2] == Approx(0.0f));
        REQUIRE(loadedVehicle->playerProtected == 1);
        REQUIRE(loadedVehicle->bulletDamage == 1);
        REQUIRE(loadedVehicle->armorPiercingDamage == 1);
        REQUIRE(loadedVehicle->grenadeDamage == 1);
        REQUIRE(loadedVehicle->projectileDamage == 1);
        REQUIRE(loadedVehicle->projectileSplashDamage == 1);
        REQUIRE(loadedVehicle->heavyExplosiveDamage == 1);
        REQUIRE(loadedVehicle->vehPhysDef.physicsEnabled == 1);
        REQUIRE(loadedVehicle->vehPhysDef.physPresetName == "vehicle_tank"s);
        REQUIRE(loadedVehicle->vehPhysDef.physPreset);
        REQUIRE(loadedVehicle->vehPhysDef.physPreset->name == "vehicle_tank"s);
        REQUIRE(loadedVehicle->vehPhysDef.accelGraphName == "tank_accel.graph"s);
        REQUIRE(loadedVehicle->vehPhysDef.steeringAxle == VEH_AXLE_ALL);
        REQUIRE(loadedVehicle->vehPhysDef.powerAxle == VEH_AXLE_ALL);
        REQUIRE(loadedVehicle->vehPhysDef.brakingAxle == VEH_AXLE_ALL);
        REQUIRE(loadedVehicle->vehPhysDef.reverseSpeed == Approx(352.0f));
        REQUIRE(loadedVehicle->vehPhysDef.maxVelocity == Approx(1760.0f));
        REQUIRE(loadedVehicle->vehPhysDef.maxPitch == Approx(60.0f));
        REQUIRE(loadedVehicle->vehPhysDef.maxRoll == Approx(60.0f));
        REQUIRE(loadedVehicle->vehPhysDef.suspensionTravelRear == Approx(0.0f));
        REQUIRE(loadedVehicle->vehPhysDef.suspensionStrengthFront == Approx(40.0f));
        REQUIRE(loadedVehicle->vehPhysDef.suspensionDampingFront == Approx(2.0f));
        REQUIRE(loadedVehicle->vehPhysDef.suspensionStrengthRear == Approx(40.0f));
        REQUIRE(loadedVehicle->vehPhysDef.suspensionDampingRear == Approx(2.0f));
        REQUIRE(loadedVehicle->vehPhysDef.frictionBraking == Approx(5.0f));
        REQUIRE(loadedVehicle->vehPhysDef.frictionCoasting == Approx(1.0f));
        REQUIRE(loadedVehicle->vehPhysDef.frictionTopSpeed == Approx(1.0f));
        REQUIRE(loadedVehicle->vehPhysDef.frictionSide == Approx(2.0f));
        REQUIRE(loadedVehicle->vehPhysDef.frictionSideRear == Approx(2.0f));
        REQUIRE(loadedVehicle->vehPhysDef.velocityDependentSlip == Approx(0.0001f));
        REQUIRE(loadedVehicle->vehPhysDef.rollStability == Approx(0.8f));
        REQUIRE(loadedVehicle->vehPhysDef.rollResistance == Approx(6160.0f));
        REQUIRE(loadedVehicle->vehPhysDef.pitchResistance == Approx(316.8f));
        REQUIRE(loadedVehicle->vehPhysDef.yawResistance == Approx(6160.0f));
        REQUIRE(loadedVehicle->vehPhysDef.uprightStrengthPitch == Approx(176.0f));
        REQUIRE(loadedVehicle->vehPhysDef.uprightStrengthRoll == Approx(616.0f));
        REQUIRE(loadedVehicle->vehPhysDef.targetAirPitch == Approx(0.0f));
        REQUIRE(loadedVehicle->vehPhysDef.airYawTorque == Approx(24.64f));
        REQUIRE(loadedVehicle->vehPhysDef.airPitchTorque == Approx(6.16f));
        REQUIRE(loadedVehicle->vehPhysDef.minimumMomentumForCollision == Approx(5280.0f));
        REQUIRE(loadedVehicle->vehPhysDef.collisionLaunchForceScale == Approx(1.75f));
        REQUIRE(loadedVehicle->vehPhysDef.wreckedMassScale == Approx(0.1f));
        REQUIRE(loadedVehicle->vehPhysDef.wreckedBodyFriction == Approx(0.6f));
        REQUIRE(loadedVehicle->vehPhysDef.minimumJoltForNotify == Approx(35200.0f));
        REQUIRE(loadedVehicle->vehPhysDef.slipThresholdFront == Approx(800.0f));
        REQUIRE(loadedVehicle->vehPhysDef.slipThresholdRear == Approx(400.0f));
        REQUIRE(loadedVehicle->vehPhysDef.slipFricScaleFront == Approx(0.2f));
        REQUIRE(loadedVehicle->vehPhysDef.slipFricScaleRear == Approx(0.1f));
        REQUIRE(loadedVehicle->vehPhysDef.slipFricRateFront == Approx(1.0f));
        REQUIRE(loadedVehicle->vehPhysDef.slipFricRateRear == Approx(1.0f));
        REQUIRE(loadedVehicle->vehPhysDef.slipYawTorque == Approx(49.28f));
        REQUIRE(loadedVehicle->boostDuration == Approx(0.0f));
        REQUIRE(loadedVehicle->boostRechargeTime == Approx(0.0f));
        REQUIRE(loadedVehicle->boostAcceleration == Approx(0.0f));
        REQUIRE(loadedVehicle->suspensionTravel == Approx(12.0f));
        REQUIRE(loadedVehicle->maxSteeringAngle == Approx(45.0f));
        REQUIRE(loadedVehicle->steeringLerp == Approx(5.0f));
        REQUIRE(loadedVehicle->minSteeringScale == Approx(1.0f));
        REQUIRE(loadedVehicle->minSteeringSpeed == Approx(0.0f));
        REQUIRE(loadedVehicle->vehHelicopterBoundsRadius == Approx(1.0));
        REQUIRE(loadedVehicle->vehHelicopterMaxSpeed == Approx(2.0));
        REQUIRE(loadedVehicle->vehHelicopterMaxSpeedVertical == Approx(3.0));
        REQUIRE(loadedVehicle->vehHelicopterMaxAccel == Approx(4.0));
        REQUIRE(loadedVehicle->vehHelicopterMaxAccelVertical == Approx(5.0));
        REQUIRE(loadedVehicle->vehHelicopterDecelerationFwd == Approx(6.0));
        REQUIRE(loadedVehicle->vehHelicopterDecelerationSide == Approx(7.0));
        REQUIRE(loadedVehicle->vehHelicopterDecelerationUp == Approx(8.0));
        REQUIRE(loadedVehicle->vehHelicopterMaxYawRate == Approx(9.0));
        REQUIRE(loadedVehicle->vehHelicopterMaxYawAccel == Approx(10.0));
        REQUIRE(loadedVehicle->vehHelicopterTiltFromVelocity == Approx(11.0));
        REQUIRE(loadedVehicle->vehHelicopterTiltFromControllerAxes == Approx(12.0));
        REQUIRE(loadedVehicle->vehHelicopterTiltFromAcceleration == Approx(13.0));
        REQUIRE(loadedVehicle->vehHelicopterTiltFromDeceleration == Approx(14.0));
        REQUIRE(loadedVehicle->vehHelicopterTiltFromFwdAndYaw_VelAtMaxTilt == Approx(15.0));
        REQUIRE(loadedVehicle->vehHelicopterTiltFromFwdAndYaw == Approx(16.0));
        REQUIRE(loadedVehicle->vehHelicopterTiltMomentum == Approx(17.0));
        REQUIRE(loadedVehicle->vehHelicopterTiltSpeed == Approx(18.0));
        REQUIRE(loadedVehicle->vehHelicopterMaxPitch == Approx(19.0));
        REQUIRE(loadedVehicle->vehHelicopterMaxRoll == Approx(20.0));
        REQUIRE(loadedVehicle->vehHelicopterHoverSpeedThreshold == Approx(21.0));
        REQUIRE(loadedVehicle->vehHelicopterJitterJerkyness == Approx(22.0));
        REQUIRE(loadedVehicle->vehHelicopterLookaheadTime == Approx(23.0));
        REQUIRE(loadedVehicle->vehHelicopterSoftCollisions == 1);
        REQUIRE(loadedVehicle->camLookEnabled == Approx(0.0f));
        REQUIRE(loadedVehicle->camRelativeControl == 1);
        REQUIRE(loadedVehicle->camLerp == Approx(8.0f));
        REQUIRE(loadedVehicle->camHeight == Approx(9.0f));
        REQUIRE(loadedVehicle->camRadius == Approx(10.0f));
        REQUIRE(loadedVehicle->camPitchInfluence == Approx(1.0f));
        REQUIRE(loadedVehicle->camYawInfluence == Approx(2.0f));
        REQUIRE(loadedVehicle->camRollInfluence == Approx(0.6f));
        REQUIRE(loadedVehicle->camFovIncrease == Approx(20.0f));
        REQUIRE(loadedVehicle->camFovOffset == Approx(4.0f));
        REQUIRE(loadedVehicle->camFovSpeed == Approx(65.0f));
        REQUIRE(loadedVehicle->camReturnSpeed == Approx(1.0));
        REQUIRE(loadedVehicle->camReturnLerp == Approx(2.0));
        REQUIRE(loadedVehicle->vehCam_UseGDT == 1);
        REQUIRE(loadedVehicle->vehCam_anglesPitch == Approx(4.0));
        REQUIRE(loadedVehicle->vehCam_anglesYaw == Approx(5.0));
        REQUIRE(loadedVehicle->vehCam_anglesRoll == Approx(6.0));
        REQUIRE(loadedVehicle->vehCam_offsetX == Approx(7.0));
        REQUIRE(loadedVehicle->vehCam_offsetY == Approx(8.0));
        REQUIRE(loadedVehicle->vehCam_offsetZ == Approx(9.0));
        REQUIRE(loadedVehicle->vehCam_radius == Approx(10.0));
        REQUIRE(loadedVehicle->vehCam_speedInfluence == Approx(11.0));
        REQUIRE(loadedVehicle->vehCam_pitchTurnRate == Approx(12.0));
        REQUIRE(loadedVehicle->vehCam_pitchClamp == Approx(13.0));
        REQUIRE(loadedVehicle->vehCam_yawTurnRate == Approx(14.0));
        REQUIRE(loadedVehicle->vehCam_yawClamp == Approx(15.0));
        REQUIRE(loadedVehicle->vehCam_zOffsetMode == VEHCAM_ZMODE_WORLD);
        REQUIRE(loadedVehicle->vehCam_anglesPitch3P == Approx(17.0));
        REQUIRE(loadedVehicle->vehCam_anglesYaw3P == Approx(18.0));
        REQUIRE(loadedVehicle->vehCam_anglesRoll3P == Approx(19.0));
        REQUIRE(loadedVehicle->vehCam_offsetX3P == Approx(20.0));
        REQUIRE(loadedVehicle->vehCam_offsetY3P == Approx(21.0));
        REQUIRE(loadedVehicle->vehCam_offsetZ3P == Approx(22.0));
        REQUIRE(loadedVehicle->vehCam_radius3P == Approx(23.0));
        REQUIRE(loadedVehicle->vehCam_speedInfluence3P == Approx(24.0));
        REQUIRE(loadedVehicle->vehCam_pitchTurnRate3P == Approx(25.0));
        REQUIRE(loadedVehicle->vehCam_pitchClamp3P == Approx(26.0));
        REQUIRE(loadedVehicle->vehCam_yawTurnRate3P == Approx(27.0));
        REQUIRE(loadedVehicle->vehCam_yawClamp3P == Approx(28.0));
        REQUIRE(loadedVehicle->vehCam_zOffsetMode3P == VEHCAM_ZMODE_VEHICLE);
        REQUIRE(loadedVehicle->turretWeaponName == "sp/bradley_turret"s);
        REQUIRE(loadedVehicle->turretHorizSpanLeft == Approx(180.0f));
        REQUIRE(loadedVehicle->turretHorizSpanRight == Approx(180.0f));
        REQUIRE(loadedVehicle->turretVertSpanUp == Approx(30.0f));
        REQUIRE(loadedVehicle->turretVertSpanDown == Approx(15.0f));
        REQUIRE(loadedVehicle->turretHorizResistLeft == Approx(1));
        REQUIRE(loadedVehicle->turretHorizResistRight == Approx(2));
        REQUIRE(loadedVehicle->turretVertResistUp == Approx(3));
        REQUIRE(loadedVehicle->turretVertResistDown == Approx(4));
        REQUIRE(loadedVehicle->turretRotRate == Approx(30.0f));
        REQUIRE(loadedVehicle->turretSpinSnd.name);
        REQUIRE(loadedVehicle->turretSpinSnd.name->soundName == "bradley_turret_spin"s);
        REQUIRE(loadedVehicle->turretStopSnd.name);
        REQUIRE(loadedVehicle->turretStopSnd.name->soundName == "bradley_turret_stop"s);
        REQUIRE(loadedVehicle->trophyEnabled == 1);
        REQUIRE(loadedVehicle->trophyRadius == Approx(450.0f));
        REQUIRE(loadedVehicle->trophyInactiveRadius == Approx(400.0f));
        REQUIRE(loadedVehicle->trophyAmmoCount == Approx(30.0f));
        REQUIRE(loadedVehicle->trophyReloadTime == Approx(1.0f));
        REQUIRE(zone.m_script_strings.Value(loadedVehicle->trophyTags[0]) == "tag_trophy_01_fx"s);
        REQUIRE(zone.m_script_strings.Value(loadedVehicle->trophyTags[1]) == "tag_trophy_02_fx"s);
        REQUIRE(zone.m_script_strings.Value(loadedVehicle->trophyTags[2]) == "tag_trophy_03_fx"s);
        REQUIRE(zone.m_script_strings.Value(loadedVehicle->trophyTags[3]) == "tag_trophy_04_fx"s);
        REQUIRE(loadedVehicle->trophyExplodeFx);
        REQUIRE(loadedVehicle->trophyExplodeFx->name == "testfx1"s);
        REQUIRE(loadedVehicle->trophyFlashFx);
        REQUIRE(loadedVehicle->trophyFlashFx->name == "testfx2"s);
        REQUIRE(loadedVehicle->compassFriendlyIcon);
        REQUIRE(loadedVehicle->compassFriendlyIcon->info.name == "hud_minimap_harrier_green"s);
        REQUIRE(loadedVehicle->compassEnemyIcon);
        REQUIRE(loadedVehicle->compassEnemyIcon->info.name == "hud_minimap_harrier_red"s);
        REQUIRE(loadedVehicle->compassFriendlyAltIcon);
        REQUIRE(loadedVehicle->compassFriendlyAltIcon->info.name == "hud_minimap_harrier_alt_green"s);
        REQUIRE(loadedVehicle->compassEnemyAltIcon);
        REQUIRE(loadedVehicle->compassEnemyAltIcon->info.name == "hud_minimap_harrier_alt_red"s);
        REQUIRE(loadedVehicle->compassIconWidth == 26);
        REQUIRE(loadedVehicle->compassIconHeight == 26);
        REQUIRE(loadedVehicle->idleLowSnd.name);
        REQUIRE(loadedVehicle->idleLowSnd.name->soundName == "veh_ambulance_idle_low"s);
        REQUIRE(loadedVehicle->idleHighSnd.name);
        REQUIRE(loadedVehicle->idleHighSnd.name->soundName == "veh_ambulance_idle_high"s);
        REQUIRE(loadedVehicle->engineLowSnd.name);
        REQUIRE(loadedVehicle->engineLowSnd.name->soundName == "veh_ambulance_engine_low"s);
        REQUIRE(loadedVehicle->engineHighSnd.name);
        REQUIRE(loadedVehicle->engineHighSnd.name->soundName == "veh_ambulance_engine_high"s);
        REQUIRE(loadedVehicle->engineSndSpeed == Approx(176.0f));
        REQUIRE(zone.m_script_strings.Value(loadedVehicle->audioOriginTag) == "foo1"s);
        REQUIRE(loadedVehicle->idleLowSndAlt.name);
        REQUIRE(loadedVehicle->idleLowSndAlt.name->soundName == "foo2"s);
        REQUIRE(loadedVehicle->idleHighSndAlt.name);
        REQUIRE(loadedVehicle->idleHighSndAlt.name->soundName == "foo3"s);
        REQUIRE(loadedVehicle->engineLowSndAlt.name);
        REQUIRE(loadedVehicle->engineLowSndAlt.name->soundName == "foo4"s);
        REQUIRE(loadedVehicle->engineHighSndAlt.name);
        REQUIRE(loadedVehicle->engineHighSndAlt.name->soundName == "foo5"s);
        REQUIRE(loadedVehicle->engineSndSpeedAlt == Approx(52.8f));
        REQUIRE(zone.m_script_strings.Value(loadedVehicle->audioOriginTagAlt) == "foo6"s);
        REQUIRE(loadedVehicle->turretSpinSndAlt.name);
        REQUIRE(loadedVehicle->turretSpinSndAlt.name->soundName == "foo7"s);
        REQUIRE(loadedVehicle->turretStopSndAlt.name);
        REQUIRE(loadedVehicle->turretStopSndAlt.name->soundName == "foo8"s);
        REQUIRE(loadedVehicle->engineStartUpSnd.name == nullptr);
        REQUIRE(loadedVehicle->engineStartUpLength == 0);
        REQUIRE(loadedVehicle->engineShutdownSnd.name == nullptr);
        REQUIRE(loadedVehicle->engineIdleSnd.name);
        REQUIRE(loadedVehicle->engineIdleSnd.name->soundName == "snowmobile_2_idle"s);
        REQUIRE(loadedVehicle->engineSustainSnd.name);
        REQUIRE(loadedVehicle->engineSustainSnd.name->soundName == "snowmobile_2_sustain"s);
        REQUIRE(loadedVehicle->engineRampUpSnd.name);
        REQUIRE(loadedVehicle->engineRampUpSnd.name->soundName == "snowmobile_2_rampup"s);
        REQUIRE(loadedVehicle->engineRampUpLength == 3263);
        REQUIRE(loadedVehicle->engineRampDownSnd.name);
        REQUIRE(loadedVehicle->engineRampDownSnd.name->soundName == "snowmobile_2_rampdown"s);
        REQUIRE(loadedVehicle->engineRampDownLength == 5262);
        REQUIRE(loadedVehicle->suspensionSoftSnd.name);
        REQUIRE(loadedVehicle->suspensionSoftSnd.name->soundName == "snowmobile_suspension_npc"s);
        REQUIRE(loadedVehicle->suspensionSoftCompression == Approx(0.3f));
        REQUIRE(loadedVehicle->suspensionHardSnd.name);
        REQUIRE(loadedVehicle->suspensionHardSnd.name->soundName == "snowmobile_suspension_heavy_npc"s);
        REQUIRE(loadedVehicle->suspensionHardCompression == Approx(0.8f));
        REQUIRE(loadedVehicle->collisionSnd.name);
        REQUIRE(loadedVehicle->collisionSnd.name->soundName == "snowmobile_collision"s);
        REQUIRE(loadedVehicle->collisionBlendSpeed == Approx(352.0f));
        REQUIRE(loadedVehicle->speedSnd.name);
        REQUIRE(loadedVehicle->speedSnd.name->soundName == "snowmobile_wind_loop"s);
        REQUIRE(loadedVehicle->speedSndBlendSpeed == Approx(1056.0f));
        REQUIRE(loadedVehicle->surfaceSndPrefix == "snowmobile_surface"s);
        REQUIRE(loadedVehicle->surfaceSndBlendSpeed == Approx(176.0f));
        REQUIRE(loadedVehicle->slideVolume == Approx(0.8f));
        REQUIRE(loadedVehicle->slideBlendSpeed == Approx(352.0f));
        REQUIRE(loadedVehicle->inAirPitch == Approx(1.3f));

        REQUIRE(loadedVehicle->vehPhysDef.topSpeed == Approx(loadedVehicle->topSpeed));
        REQUIRE(loadedVehicle->vehPhysDef.suspensionTravelFront == Approx(loadedVehicle->suspensionTravel));

        REQUIRE(std::string(loadedVehicle->surfaceSnds[SURF_TYPE_DEFAULT].name->soundName) == "snowmobile_surface_default");
        REQUIRE(std::string(loadedVehicle->surfaceSnds[SURF_TYPE_SLUSH].name->soundName) == "snowmobile_surface_slush");

        REQUIRE(loadedAssetInfo->m_dependencies.size() == 8u);
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, physPresetInfo) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, weaponInfo) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, material0) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, material1) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, material2) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, material3) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, fx0) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, fx1) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(loadedAssetInfo->m_used_script_strings.size() == 6u);
        REQUIRE(loadedAssetInfo->m_indirect_asset_references.size() == 51u);
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound0) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound1) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound2) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound3) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound4) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound5) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound6) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound7) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound8) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound9) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound10) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound11) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound12) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound13) != loadedAssetInfo->m_indirect_asset_references.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_indirect_asset_references, sound14) != loadedAssetInfo->m_indirect_asset_references.end());
    }
} // namespace
