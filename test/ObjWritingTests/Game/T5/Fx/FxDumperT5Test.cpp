#include "Game/T5/Fx/FxEffectDefDumperT5.h"
#include "Game/T5/Fx/FxImpactTableDumperT5.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace T5;

namespace
{
    bool DumpsImpactTableTo(const std::string& zoneName, const std::string& assetName, const std::string& fileName)
    {
        FxImpactEntry entries[21]{};
        FxImpactTable impactTable{.name = assetName.c_str(), .table = entries};

        Zone dumpingZone(zoneName, 0, GameId::T5, GamePlatform::PC);
        dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<FxImpactTable>>(ASSET_TYPE_IMPACT_FX, impactTable.name, &impactTable));

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        fx::ImpactTableDumperT5 dumper;
        dumper.Dump(dumpingContext);

        return dumpingOutput.GetMockedFile(fileName) != nullptr;
    }

    TEST_CASE("FxEffectDef writer reconstructs T5 iwfx 3 fields", "[t5][fx]")
    {
        Material material{};
        material.info.name = "fx_test_rotated";

        FxElemDef elems[2]{};
        auto& rotated = elems[0];
        rotated.flags = static_cast<int>(FX_ELEM_USE_ITEM_CLIP | FX_ELEM_IS_MATURE_CONTENT | FX_ELEM_SPAWN_DISTRIBUTIVE_X | FX_ELEM_SPAWN_DISTRIBUTIVE_Y
                                         | FX_ELEM_SPAWN_RELATIVE_TO_EFFECT | FX_ELEM_SPAWN_RELATIVE_TYPE_1 | FX_ELEM_SPAWN_RELATIVE_TYPE_4
                                         | FX_ELEM_FLAME_CHUNK | FX_ELEM_HAS_REFLECTION | FX_ELEM_USE_ROTATION_AXIS | FX_ELEM_USE_GAUSSIAN_CLOUD
                                         | FX_ELEM_USE_BILLBOARD_PIVOT | FX_ELEM_USE_WORLD_UP | FX_ELEM_ALIGN_VIEWPOINT);
        // Vec4PackQuat({sqrt(0.5), 0, 0, sqrt(0.5)}).
        rotated.rotationAxis = 0x6FF80000u;
        rotated.atlas.behavior = static_cast<char>(FX_ATLAS_START_FIXED_RANGE | FX_ATLAS_PLAY_OVER_LIFE);
        rotated.atlas.loopCount = 4;
        rotated.atlas.entryCountAndIndexRange = static_cast<uint16_t>((5u << 9u) | 8u);
        rotated.windInfluence = 0.25f;
        rotated.elemType = FX_ELEM_TYPE_SPRITE_ROTATED;
        rotated.visualCount = 1;
        rotated.visuals.instance.material = &material;
        rotated.effectAttached.name = ",test/attached";
        rotated.alphaFadeTimeMsec = 123;
        rotated.maxWindStrength = 456;
        rotated.lifespanAtMaxWind = 789;
        rotated.spawnIntervalAtMaxWind = 321;
        rotated.u.billboard.topWidth = 0.25f;
        rotated.u.billboard.bottomWidth = 0.75f;
        rotated.spawnSound.spawnSound = "test_spawn_sound";
        rotated.billboardPivot[0] = 0.5f;
        rotated.billboardPivot[1] = -1.0f;

        auto& cloud = elems[1];
        cloud.elemType = FX_ELEM_TYPE_CLOUD;
        cloud.u.cloudDensityRange = {3, 5};

        FxEffectDef effect{};
        effect.name = "test/effect";
        effect.flags = static_cast<char>(FX_EFFECT_HAS_LIGHTING | FX_EFFECT_HAS_MARKS | FX_EFFECT_USE_BOUNDING_BOX | FX_EFFECT_USE_LOCAL_BOUNDING_BOX);
        effect.efPriority = 110;
        effect.elemDefCountOneShot = 2;
        effect.elemDefs = elems;
        effect.boundingBoxDim[0] = 1.0f;
        effect.boundingBoxDim[1] = 2.0f;
        effect.boundingBoxDim[2] = 3.0f;
        effect.boundingSphere[0] = 4.0f;
        effect.boundingSphere[1] = 5.0f;
        effect.boundingSphere[2] = 6.0f;

        Zone dumpingZone("DumpingZone", 0, GameId::T5, GamePlatform::PC);
        dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<FxEffectDef>>(ASSET_TYPE_FX, effect.name, &effect));

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        fx::EffectDefDumperT5 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = dumpingOutput.GetMockedFile("fx/test/effect.efx");
        REQUIRE(dumpedFile != nullptr);
        const auto text = dumpedFile->AsString();

        REQUIRE(text.starts_with("iwfx 3\n"));
        REQUIRE(text.find("efFlags efUseBoundingBox efLocalBoundingBox;") != std::string::npos);
        REQUIRE(text.find("efPriority 110;") != std::string::npos);
        REQUIRE(text.find("efBoundingBoxDim 1 2 3;") != std::string::npos);
        REQUIRE(text.find("efBoundingBoxCentre 4 5 6;") != std::string::npos);
        REQUIRE(text.find("editorFlags playAttached;") != std::string::npos);
        REQUIRE(text.find("flags useItemClip isMatureContent spawnDistributiveX spawnDistributiveY spawnRelative spawnRelativeType0 spawnRelativeType1 "
                          "spawnRelativeType4 spawnOffsetNone runRelToWorld flamechunk hasReflection useRotationAxis useGaussianCloud useBillboardPivot "
                          "useWorldUp alignViewpoint;")
                != std::string::npos);
        REQUIRE(text.find("rotationAxis 0.707106769 0 0 0.707106769;") != std::string::npos);
        REQUIRE(text.find("atlasBehavior startFixedRange playOverLife;") != std::string::npos);
        REQUIRE(text.find("atlasLoopCount 3;") != std::string::npos);
        REQUIRE(text.find("atlasEntryCount 8;") != std::string::npos);
        REQUIRE(text.find("atlasIndexRange 5;") != std::string::npos);
        REQUIRE(text.find("attachment \"test/attached\";") != std::string::npos);
        REQUIRE(text.find("alphafadetimemsec 123;") != std::string::npos);
        REQUIRE(text.find("maxwind_mag 456;") != std::string::npos);
        REQUIRE(text.find("maxwind_life 789;") != std::string::npos);
        REQUIRE(text.find("maxwind_interval 321;") != std::string::npos);
        REQUIRE(text.find("billboardTopWidth 0.25;") != std::string::npos);
        REQUIRE(text.find("billboardBottomWidth 0.75;") != std::string::npos);
        REQUIRE(text.find("\t\t\"test_spawn_sound\"\n") != std::string::npos);
        REQUIRE(text.find("cloudDensity 3 5;") != std::string::npos);
        REQUIRE(text.find("billboardPivot 0.25 0.5;") != std::string::npos);
        REQUIRE(text.find("rotatedSprite\n\t{\n\t\t\"fx_test_rotated\"") != std::string::npos);
    }

    TEST_CASE("FxImpactTable dumper uses T5 impact ordering and flesh groups", "[t5][fx]")
    {
        FxEffectDef armorPiercingEffect{.name = "impacts/armor_piercing"};
        FxEffectDef extremeEffect{.name = "impacts/extreme"};
        FxEffectDef shotgunEffect{.name = "impacts/shotgun"};
        FxEffectDef physicsEffect{.name = "impacts/physics"};
        FxEffectDef boltEffect{.name = "impacts/bolt_flesh"};
        FxEffectDef bladeEffect{.name = "impacts/blade_flesh"};

        FxImpactEntry entries[21]{};
        entries[5].nonflesh[SURF_TYPE_DEFAULT] = &shotgunEffect;
        entries[7].nonflesh[SURF_TYPE_DEFAULT] = &armorPiercingEffect;
        entries[9].nonflesh[SURF_TYPE_DEFAULT] = &extremeEffect;
        entries[18].nonflesh[SURF_TYPE_PLAYER] = &physicsEffect;
        entries[19].flesh[0] = &boltEffect;
        entries[20].flesh[3] = &bladeEffect;

        FxImpactTable impactTable{.name = "mp_test", .table = entries};
        Zone dumpingZone("mp_test", 0, GameId::T5, GamePlatform::PC);
        dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<FxImpactTable>>(ASSET_TYPE_IMPACT_FX, impactTable.name, &impactTable));

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        fx::ImpactTableDumperT5 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = dumpingOutput.GetMockedFile("fx/maps/mp_test/iw_impacts.csv");
        REQUIRE(dumpedFile != nullptr);
        const auto text = dumpedFile->AsString();

        REQUIRE(std::count(text.begin(), text.end(), '\n') == 704);
        REQUIRE(text.find("bullet_ap_normal,default,impacts/armor_piercing\n") != std::string::npos);
        REQUIRE(text.find("bullet_xtreme_normal,default,impacts/extreme\n") != std::string::npos);
        REQUIRE(text.find("shotgun_normal,default,impacts/shotgun\n") != std::string::npos);
        REQUIRE(text.find("physics_impact,player,impacts/physics\n") != std::string::npos);
        REQUIRE(text.find("bolt,flesh_body_nonfatal,impacts/bolt_flesh\n") != std::string::npos);
        REQUIRE(text.find("blade,flesh_head_fatal,impacts/blade_flesh\n") != std::string::npos);
        REQUIRE(text.find("grenade_bounce,flesh_body_nonfatal,") == std::string::npos);

        const auto armorPiercingPosition = text.find("bullet_ap_normal,default,");
        const auto extremePosition = text.find("bullet_xtreme_normal,default,");
        const auto shotgunPosition = text.find("shotgun_normal,default,");
        const auto physicsPosition = text.find("physics_impact,default,");
        const auto mortarPosition = text.find("mortar_shell,default,");
        const auto tankPosition = text.find("tank_shell,default,");
        REQUIRE(armorPiercingPosition < extremePosition);
        REQUIRE(extremePosition < shotgunPosition);
        REQUIRE(physicsPosition < mortarPosition);
        REQUIRE(mortarPosition < tankPosition);
    }

    TEST_CASE("FxImpactTable dumper reconstructs internal T5 source paths", "[t5][fx]")
    {
        SECTION("The common table is written to the root FX directory")
        {
            REQUIRE(DumpsImpactTableTo("common", "ImpactFx", "fx/iw_impacts.csv"));
        }

        SECTION("The multiplayer common table uses the mp_maps directory")
        {
            REQUIRE(DumpsImpactTableTo("common_mp", "ImpactFx", "fx/maps/mp_maps/iw_impacts.csv"));
        }

        SECTION("The zombie common table uses the zombie directory")
        {
            REQUIRE(DumpsImpactTableTo("common_zombie", "ImpactFx", "fx/maps/zombie/iw_impacts.csv"));
        }

        SECTION("A map table uses the zone name")
        {
            REQUIRE(DumpsImpactTableTo("mp_array", "ImpactFx", "fx/maps/mp_array/iw_impacts.csv"));
        }

        SECTION("An empty load-object table name is handled the same way")
        {
            REQUIRE(DumpsImpactTableTo("common_mp", "", "fx/maps/mp_maps/iw_impacts.csv"));
        }

        SECTION("An explicit table name is preserved")
        {
            REQUIRE(DumpsImpactTableTo("common_mp", "custom", "fx/maps/custom/iw_impacts.csv"));
        }
    }
} // namespace
