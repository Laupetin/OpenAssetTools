#include "Game/IW3/Fx/FxEffectDefLoaderIW3.h"
#include "Game/T4/Fx/FxEffectDefLoaderT4.h"
#include "Game/T5/Fx/FxEffectDefLoaderT5.h"
#include "SearchPath/MockSearchPath.h"

#include <array>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <string_view>

using namespace Catch;

namespace
{
    constexpr auto SCALAR_GRAPH = " { { 0 0.5 1 0.5 } { 0 1 1 1 } };";

    void AddScalarGraph(std::string& effect, const std::string_view name, const int scale)
    {
        effect.append(name);
        effect.append(" ");
        effect.append(std::to_string(scale));
        effect.append(SCALAR_GRAPH);
    }

    std::string MakeEffect(const int version, const std::string_view effectFields, const std::string_view elementFields, const std::string_view visual)
    {
        std::string effect = "// stock fx source\niwfx " + std::to_string(version) + "\n";
        effect.append(effectFields);
        effect.append(R"(
{
    name "test element";
    editorFlags looping useRandColor useRandAlpha useRandSize0 useRandRotDelta useRandVel1 boundingSphere;
    flags spawnRelative;
    spawnRange 2 -4;
    spawnLooping 250 3;
    spawnOneShot 1 0;
    spawnDelayMsec 20 10;
    lifeSpanMsec 1000 0;
    spawnOrgX 1 2;
    spawnAngleYaw 180 0;
    initialRot 90 0;
    gravity 100 50;
    elasticity 0.25 0.5;
    atlasBehavior startFixed;
    atlasIndex 0;
    atlasFps 0;
    atlasLoopCount 0;
    atlasColIndexBits 0;
    atlasRowIndexBits 0;
    atlasEntryCount 1;
)");

        constexpr std::array VELOCITY_GRAPHS{
            "velGraph0X",
            "velGraph0Y",
            "velGraph0Z",
            "velGraph1X",
            "velGraph1Y",
            "velGraph1Z",
        };
        for (const auto graph : VELOCITY_GRAPHS)
            AddScalarGraph(effect, graph, graph == std::string_view("velGraph0X") ? 100 : 0);

        AddScalarGraph(effect, "rotGraph", 180);
        AddScalarGraph(effect, "sizeGraph0", 4);
        AddScalarGraph(effect, "sizeGraph1", 6);
        AddScalarGraph(effect, "scaleGraph", 2);
        effect.append(R"(
    colorGraph 1 { { 0 1 0.5 0.25 1 1 0.5 0.25 } { 0 0.25 0.5 1 1 0.25 0.5 1 } };
    alphaGraph 1 { { 0 1 1 1 } { 0 0.5 1 0.5 } };
    lightingFrac 0.5;
    collOffset 1 2 3;
    collRadius 4;
    fxOnImpact "";
    fxOnDeath "";
    emission "";
    emitDist 0 0;
    emitDistVariance 0 0;
)");
        effect.append(elementFields);
        effect.append(visual);
        effect.append(";\n}\n");
        return effect;
    }

    TEST_CASE("FxEffectDef loader converts stock IW3 fields", "[iw3][fx][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("fx/test.efx", MakeEffect(1, "", "", "light"));

        Zone zone("MockZone", 0, GameId::IW3, GamePlatform::PC);
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = fx::CreateLoaderIW3(zone.Memory(), searchPath);
        const auto result = loader->CreateAsset("test", context);

        REQUIRE(result.HasBeenSuccessful());
        const auto* assetInfo = reinterpret_cast<XAssetInfo<IW3::FxEffectDef>*>(result.GetAssetInfo());
        const auto* effect = assetInfo->Asset();
        REQUIRE(std::string(effect->name) == "test");
        REQUIRE(effect->elemDefCountLooping == 1);
        REQUIRE(effect->elemDefCountOneShot == 0);
        REQUIRE(effect->elemDefCountEmission == 0);
        REQUIRE(effect->msecLoopingLife == 500);

        const auto& element = effect->elemDefs[0];
        REQUIRE(element.elemType == IW3::FX_ELEM_TYPE_OMNI_LIGHT);
        REQUIRE(element.visualCount == 1);
        REQUIRE(element.spawn.looping.intervalMsec == 250);
        REQUIRE(element.spawn.looping.count == 3);
        REQUIRE(element.spawnRange.base == Approx(-2.0f));
        REQUIRE(element.spawnRange.amplitude == Approx(4.0f));
        REQUIRE(element.initialRotation.base == Approx(1.5707963f));
        REQUIRE(element.gravity.base == Approx(1.0f));
        REQUIRE(element.gravity.amplitude == Approx(0.5f));
        REQUIRE((element.flags & IW3::FX_ELEM_HAS_GRAVITY) != 0);
        REQUIRE((element.flags & IW3::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL) != 0);
        REQUIRE(element.collMins[0] == Approx(-3.0f));
        REQUIRE(element.collMaxs[2] == Approx(7.0f));
        REQUIRE(element.lightingFrac == 127u);
        REQUIRE(element.visSamples[0].base.size[0] == Approx(1.0f));
        REQUIRE(element.visSamples[0].base.color[0] == 64u);
        REQUIRE(element.visSamples[0].base.color[1] == 128u);
        REQUIRE(element.visSamples[0].base.color[2] == 255u);
    }

    TEST_CASE("FxEffectDef loader converts T4 line fields", "[t4][fx][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("fx/test.efx", MakeEffect(2, "", "    windinfluence 0.75;\n    efPriority 7;\n", "    line { }"));

        Zone zone("MockZone", 0, GameId::T4, GamePlatform::PC);
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = fx::CreateLoaderT4(zone.Memory(), searchPath);
        const auto result = loader->CreateAsset("test", context);

        REQUIRE(result.HasBeenSuccessful());
        const auto* assetInfo = reinterpret_cast<XAssetInfo<T4::FxEffectDef>*>(result.GetAssetInfo());
        const auto* effect = assetInfo->Asset();
        REQUIRE(effect->efPriority == 7);
        REQUIRE(effect->elemDefs[0].elemType == T4::FX_ELEM_TYPE_LINE);
        REQUIRE(effect->elemDefs[0].windInfluence == Approx(0.75f));
        REQUIRE(effect->elemDefs[0].visSamples[0].base.size[1] == Approx(1.5f));
        REQUIRE(effect->elemDefs[0].visSamples[0].base.scale == Approx(1.0f));
    }

    TEST_CASE("FxEffectDef loader converts T5 version 3 fields", "[t5][fx][assetloader]")
    {
        constexpr auto EFFECT_FIELDS = R"(
efFlags efUseBoundingBox efLocalBoundingBox;
efPriority 9;
efBoundingBoxDim 10 20 30;
efBoundingBoxCentre 1 2 3;
)";
        constexpr auto ELEMENT_FIELDS = R"(
    flags nonUniformScale useRotationAxis useBillboardPivot;
    rotationAxis 0 0 1 0;
    windinfluence 0.25;
    atlasIndexRange 4;
    attachment "";
    alphafadetimemsec 250;
    maxwind_mag 12;
    maxwind_life 34;
    maxwind_interval 56;
    billboardTopWidth 0.25;
    billboardBottomWidth 0.75;
    elemSpawnSound { "test_sound" };
    billboardPivot 2 -3;
)";

        MockSearchPath searchPath;
        searchPath.AddFileData("fx/test.efx", MakeEffect(3, EFFECT_FIELDS, ELEMENT_FIELDS, "    rotatedSprite { }"));

        Zone zone("MockZone", 0, GameId::T5, GamePlatform::PC);
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = fx::CreateLoaderT5(zone.Memory(), searchPath);
        const auto result = loader->CreateAsset("test", context);

        REQUIRE(result.HasBeenSuccessful());
        const auto* assetInfo = reinterpret_cast<XAssetInfo<T5::FxEffectDef>*>(result.GetAssetInfo());
        const auto* effect = assetInfo->Asset();
        REQUIRE(effect->efPriority == 9);
        REQUIRE((effect->flags & T5::FX_EFFECT_USE_BOUNDING_BOX) != 0);
        REQUIRE((effect->flags & T5::FX_EFFECT_USE_LOCAL_BOUNDING_BOX) != 0);
        REQUIRE((effect->flags & T5::FX_EFFECT_HAS_LIGHTING) != 0);
        REQUIRE(effect->boundingBoxDim[2] == Approx(30.0f));
        REQUIRE(effect->boundingSphere[0] == Approx(1.0f));
        REQUIRE(effect->boundingSphere[3] == Approx(42.426411f));

        const auto& element = effect->elemDefs[0];
        REQUIRE(element.elemType == T5::FX_ELEM_TYPE_SPRITE_ROTATED);
        REQUIRE(element.rotationAxis == 0x20000000u);
        REQUIRE(element.alphaFadeTimeMsec == 250u);
        REQUIRE(element.maxWindStrength == 12u);
        REQUIRE(element.lifespanAtMaxWind == 34u);
        REQUIRE(element.spawnIntervalAtMaxWind == 56u);
        REQUIRE(element.u.billboard.topWidth == Approx(0.25f));
        REQUIRE(element.u.billboard.bottomWidth == Approx(0.75f));
        REQUIRE(std::string(element.spawnSound.spawnSound) == "test_sound");
        REQUIRE(element.billboardPivot[0] == Approx(4.0f));
        REQUIRE(element.billboardPivot[1] == Approx(6.0f));
    }
} // namespace
