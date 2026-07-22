#include "Game/IW5/Vehicle/RawLoaderVehicleIW5.h"
#include "Game/IW5/Vehicle/VehicleDumperIW5.h"
#include "InfoString/InfoString.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "ZoneLoading.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <sstream>
#include <string>

using namespace IW5;

namespace
{
    constexpr auto VEHICLE_NAME = "test_vehicle_iw5";
    constexpr auto PHYS_PRESET_NAME = "test_phys_preset_iw5";
    constexpr auto WEAPON_NAME = "test_turret_weapon_iw5";
    constexpr auto EXPLODE_FX_NAME = "test_trophy_explode_fx";
    constexpr auto FLASH_FX_NAME = "test_trophy_flash_fx";
    constexpr auto MATERIAL_NAME = "test_compass_alt_material";
    constexpr auto ALT_IDLE_SOUND_NAME = "test_alt_idle_sound";
    constexpr auto SURFACE_SOUND_PREFIX = "test_iw5_surface";

    struct TestVehicle
    {
        std::unique_ptr<PhysPreset> physPreset;
        std::unique_ptr<WeaponCompleteDef> weapon;
        std::unique_ptr<FxEffectDef> explodeFx;
        std::unique_ptr<FxEffectDef> flashFx;
        std::unique_ptr<Material> material;
        std::unique_ptr<snd_alias_list_name> altIdleSound;
        std::unique_ptr<VehicleDef> vehicle;
    };

    TestVehicle CreateTestVehicle()
    {
        TestVehicle result{
            std::make_unique<PhysPreset>(),
            std::make_unique<WeaponCompleteDef>(),
            std::make_unique<FxEffectDef>(),
            std::make_unique<FxEffectDef>(),
            std::make_unique<Material>(),
            std::make_unique<snd_alias_list_name>(),
            std::make_unique<VehicleDef>(),
        };

        result.physPreset->name = PHYS_PRESET_NAME;
        result.weapon->szInternalName = WEAPON_NAME;
        result.explodeFx->name = EXPLODE_FX_NAME;
        result.flashFx->name = FLASH_FX_NAME;
        result.material->info.name = MATERIAL_NAME;
        result.altIdleSound->soundName = ALT_IDLE_SOUND_NAME;

        result.vehicle->name = VEHICLE_NAME;
        result.vehicle->type = VEH_UGV;
        result.vehicle->health = 900;
        result.vehicle->topSpeed = 176.0f;
        result.vehicle->suspensionTravel = 5.5f;
        result.vehicle->fakeBodyStabilizer = true;
        result.vehicle->vehPhysDef.physicsEnabled = true;
        result.vehicle->vehPhysDef.physPresetName = PHYS_PRESET_NAME;
        result.vehicle->vehPhysDef.physPreset = result.physPreset.get();
        result.vehicle->vehPhysDef.steeringAxle = VEH_AXLE_ALL;
        result.vehicle->vehHelicopterBoundsRadius = 123.25f;
        result.vehicle->camRelativeControl = true;
        result.vehicle->vehCam_zOffsetMode = VEHCAM_ZMODE_VIEW;
        result.vehicle->vehCam_zOffsetMode3P = VEHCAM_ZMODE_VEHICLE;
        result.vehicle->turretWeaponName = WEAPON_NAME;
        result.vehicle->turretWeapon = result.weapon.get();
        result.vehicle->turretHorizResistLeft = 27.5f;
        result.vehicle->trophyExplodeFx = result.explodeFx.get();
        result.vehicle->trophyFlashFx = result.flashFx.get();
        result.vehicle->compassFriendlyAltIcon = result.material.get();
        result.vehicle->idleLowSndAlt.name = result.altIdleSound.get();
        result.vehicle->surfaceSndPrefix = SURFACE_SOUND_PREFIX;

        return result;
    }

    std::string DumpVehicle(VehicleDef& vehicle)
    {
        Zone dumpingZone("DumpingZone", 0, GameId::IW5, GamePlatform::PC);
        vehicle.trophyTags[0] = dumpingZone.m_script_strings.AddOrGetScriptString("tag_trophy_iw5");
        vehicle.audioOriginTag = dumpingZone.m_script_strings.AddOrGetScriptString("tag_audio_origin");
        vehicle.audioOriginTagAlt = dumpingZone.m_script_strings.AddOrGetScriptString("tag_audio_origin_alt");
        dumpingZone.m_pools.AddAsset(ASSET_TYPE_VEHICLE, vehicle.name, &vehicle, {}, {}, {});

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        vehicle::DumperIW5 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = dumpingOutput.GetMockedFile("vehicles/test_vehicle_iw5");
        REQUIRE(dumpedFile != nullptr);
        return dumpedFile->AsString();
    }

    TEST_CASE("Vehicle dumper serializes IW5 fields", "[iw5][vehicle][system]")
    {
        auto testVehicle = CreateTestVehicle();
        std::istringstream dumpedStream(DumpVehicle(*testVehicle.vehicle));

        InfoString infoString;
        REQUIRE(infoString.FromStream("VEHICLEFILE", dumpedStream));
        REQUIRE(infoString.GetValueForKey("type") == "ugv");
        REQUIRE(infoString.GetValueForKey("topSpeed") == "10.000000");
        REQUIRE(infoString.GetValueForKey("fakeBodyStabilizer") == "1");
        REQUIRE(infoString.GetValueForKey("physicsPreset") == PHYS_PRESET_NAME);
        REQUIRE(infoString.GetValueForKey("vehHelicopterBoundsRadius") == "123.25");
        REQUIRE(infoString.GetValueForKey("camRelativeControl") == "1");
        REQUIRE(infoString.GetValueForKey("vehCam_zOffsetMode") == "view");
        REQUIRE(infoString.GetValueForKey("vehCam_zOffsetMode3P") == "vehicle");
        REQUIRE(infoString.GetValueForKey("turretHorizResistLeft") == "27.5");
        REQUIRE(infoString.GetValueForKey("trophyExplodeFx") == EXPLODE_FX_NAME);
        REQUIRE(infoString.GetValueForKey("trophyFlashFx") == FLASH_FX_NAME);
        REQUIRE(infoString.GetValueForKey("compassFriendlyAltIcon") == MATERIAL_NAME);
        REQUIRE(infoString.GetValueForKey("audioOriginTag") == "tag_audio_origin");
        REQUIRE(infoString.GetValueForKey("audioOriginTagAlt") == "tag_audio_origin_alt");
        REQUIRE(infoString.GetValueForKey("lowIdleSndAlt") == ALT_IDLE_SOUND_NAME);
    }

    TEST_CASE("Vehicle loader restores dumped raw asset (IW5)", "[iw5][vehicle][system]")
    {
        auto testVehicle = CreateTestVehicle();
        MockSearchPath loadingSearchPath;
        loadingSearchPath.AddFileData("vehicles/test_vehicle_iw5", DumpVehicle(*testVehicle.vehicle));

        Zone loadingZone("LoadingZone", 0, GameId::IW5, GamePlatform::PC);
        auto* physPresetInfo =
            loadingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<PhysPreset>>(ASSET_TYPE_PHYSPRESET, PHYS_PRESET_NAME, testVehicle.physPreset.get()));
        auto* weaponInfo =
            loadingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<WeaponCompleteDef>>(ASSET_TYPE_WEAPON, WEAPON_NAME, testVehicle.weapon.get()));
        auto* explodeFxInfo =
            loadingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<FxEffectDef>>(ASSET_TYPE_FX, EXPLODE_FX_NAME, testVehicle.explodeFx.get()));
        auto* flashFxInfo = loadingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<FxEffectDef>>(ASSET_TYPE_FX, FLASH_FX_NAME, testVehicle.flashFx.get()));
        auto* materialInfo =
            loadingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<Material>>(ASSET_TYPE_MATERIAL, MATERIAL_NAME, testVehicle.material.get()));

        AssetCreatorCollection creatorCollection(loadingZone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext loadingContext(loadingZone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = vehicle::CreateRawLoaderIW5(loadingZone.Memory(), loadingSearchPath, loadingZone);
        const auto result = loader->CreateAsset(VEHICLE_NAME, loadingContext);

        REQUIRE(result.HasBeenSuccessful());
        const auto* loadedAssetInfo = reinterpret_cast<XAssetInfo<VehicleDef>*>(result.GetAssetInfo());
        const auto* loadedVehicle = loadedAssetInfo->Asset();

        REQUIRE(std::string(loadedVehicle->name) == VEHICLE_NAME);
        REQUIRE(loadedVehicle->type == VEH_UGV);
        REQUIRE(loadedVehicle->topSpeed == 176.0f);
        REQUIRE(loadedVehicle->vehPhysDef.topSpeed == loadedVehicle->topSpeed);
        REQUIRE(loadedVehicle->vehPhysDef.suspensionTravelFront == loadedVehicle->suspensionTravel);
        REQUIRE(loadedVehicle->fakeBodyStabilizer == 1);
        REQUIRE(loadedVehicle->vehPhysDef.physPreset == testVehicle.physPreset.get());
        REQUIRE(loadedVehicle->vehHelicopterBoundsRadius == 123.25f);
        REQUIRE(loadedVehicle->camRelativeControl == 1);
        REQUIRE(loadedVehicle->vehCam_zOffsetMode == VEHCAM_ZMODE_VIEW);
        REQUIRE(loadedVehicle->vehCam_zOffsetMode3P == VEHCAM_ZMODE_VEHICLE);
        REQUIRE(loadedVehicle->turretWeapon == testVehicle.weapon.get());
        REQUIRE(loadedVehicle->turretHorizResistLeft == 27.5f);
        REQUIRE(loadedVehicle->trophyExplodeFx == testVehicle.explodeFx.get());
        REQUIRE(loadedVehicle->trophyFlashFx == testVehicle.flashFx.get());
        REQUIRE(loadedVehicle->compassFriendlyAltIcon == testVehicle.material.get());
        REQUIRE(loadingZone.m_script_strings[loadedVehicle->trophyTags[0]] == "tag_trophy_iw5");
        REQUIRE(loadingZone.m_script_strings[loadedVehicle->audioOriginTag] == "tag_audio_origin");
        REQUIRE(loadingZone.m_script_strings[loadedVehicle->audioOriginTagAlt] == "tag_audio_origin_alt");
        REQUIRE(std::string(loadedVehicle->idleLowSndAlt.name->soundName) == ALT_IDLE_SOUND_NAME);
        REQUIRE(std::string(loadedVehicle->surfaceSnds[0].name->soundName) == "test_iw5_surface_default");
        REQUIRE(std::string(loadedVehicle->surfaceSnds[30].name->soundName) == "test_iw5_surface_slush");

        REQUIRE(loadedAssetInfo->m_dependencies.size() == 5u);
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, physPresetInfo) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, weaponInfo) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, explodeFxInfo) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, flashFxInfo) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(std::ranges::find(loadedAssetInfo->m_dependencies, materialInfo) != loadedAssetInfo->m_dependencies.end());
        REQUIRE(loadedAssetInfo->m_used_script_strings.size() == 3u);
        REQUIRE(loadedAssetInfo->m_indirect_asset_references.size() == 32u);
    }
} // namespace
