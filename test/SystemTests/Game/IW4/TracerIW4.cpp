#include "Game/IW4/Tracer/RawLoaderTracerIW4.h"
#include "Game/IW4/Tracer/TracerDumperIW4.h"
#include "InfoString/InfoString.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "ZoneLoading.h"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <sstream>
#include <string>

using namespace IW4;

namespace
{
    constexpr auto TRACER_NAME = "test_tracer";
    constexpr auto MATERIAL_NAME = "tracer_material";

    struct TestTracer
    {
        std::unique_ptr<Material> material;
        std::unique_ptr<TracerDef> tracer;
    };

    TestTracer CreateTestTracer()
    {
        TestTracer result{std::make_unique<Material>(), std::make_unique<TracerDef>()};
        result.material->info.name = ",tracer_material";

        result.tracer->name = TRACER_NAME;
        result.tracer->material = result.material.get();
        result.tracer->drawInterval = 7;
        result.tracer->speed = 8.25f;
        result.tracer->beamLength = 9.5f;
        result.tracer->beamWidth = 10.75f;
        result.tracer->screwRadius = 11.25f;
        result.tracer->screwDist = 12.5f;

        for (auto colorIndex = 0u; colorIndex < 5u; colorIndex++)
        {
            for (auto componentIndex = 0u; componentIndex < 4u; componentIndex++)
                result.tracer->colors[colorIndex][componentIndex] = static_cast<float>(13u + colorIndex * 4u + componentIndex);
        }

        return result;
    }

    std::string DumpTracer(TracerDef& tracerDef)
    {
        Zone dumpingZone("DumpingZone", 0, GameId::IW4, GamePlatform::PC);
        dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<TracerDef>>(ASSET_TYPE_TRACER, tracerDef.name, &tracerDef));

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        tracer::DumperIW4 dumper;
        dumper.Dump(dumpingContext);

        const auto* dumpedFile = dumpingOutput.GetMockedFile("tracer/test_tracer");
        REQUIRE(dumpedFile != nullptr);

        return dumpedFile->AsString();
    }

    void RequireTracerValues(const TracerDef& actual, const TracerDef& expected)
    {
        REQUIRE(std::string(actual.name) == expected.name);
        REQUIRE(actual.drawInterval == expected.drawInterval);
        REQUIRE(actual.speed == expected.speed);
        REQUIRE(actual.beamLength == expected.beamLength);
        REQUIRE(actual.beamWidth == expected.beamWidth);
        REQUIRE(actual.screwRadius == expected.screwRadius);
        REQUIRE(actual.screwDist == expected.screwDist);

        for (auto colorIndex = 0u; colorIndex < 5u; colorIndex++)
        {
            for (auto componentIndex = 0u; componentIndex < 4u; componentIndex++)
                REQUIRE(actual.colors[colorIndex][componentIndex] == expected.colors[colorIndex][componentIndex]);
        }
    }

    TEST_CASE("Tracer dumper serializes raw fields (IW4)", "[iw4][tracer][system]")
    {
        auto testTracer = CreateTestTracer();
        std::istringstream dumpedStream(DumpTracer(*testTracer.tracer));

        InfoString infoString;
        REQUIRE(infoString.FromStream("TRACER", dumpedStream));
        REQUIRE(infoString.GetValueForKey("material") == MATERIAL_NAME);
        REQUIRE(infoString.GetValueForKey("drawInterval") == "7");
        REQUIRE(infoString.GetValueForKey("speed") == "8.25");
        REQUIRE(infoString.GetValueForKey("beamLength") == "9.5");
        REQUIRE(infoString.GetValueForKey("beamWidth") == "10.75");
        REQUIRE(infoString.GetValueForKey("screwRadius") == "11.25");
        REQUIRE(infoString.GetValueForKey("screwDist") == "12.5");

        constexpr std::array colorFields{
            "colorR0", "colorG0", "colorB0", "colorA0", "colorR1", "colorG1", "colorB1", "colorA1", "colorR2", "colorG2",
            "colorB2", "colorA2", "colorR3", "colorG3", "colorB3", "colorA3", "colorR4", "colorG4", "colorB4", "colorA4",
        };
        for (auto fieldIndex = 0u; fieldIndex < colorFields.size(); fieldIndex++)
        {
            REQUIRE(infoString.GetValueForKey(colorFields[fieldIndex]) == std::to_string(13u + fieldIndex));
        }
    }

    TEST_CASE("Tracer loader restores dumped raw asset (IW4)", "[iw4][tracer][system]")
    {
        auto testTracer = CreateTestTracer();
        MockSearchPath loadingSearchPath;
        loadingSearchPath.AddFileData("tracer/test_tracer", DumpTracer(*testTracer.tracer));

        Zone loadingZone("LoadingZone", 0, GameId::IW4, GamePlatform::PC);
        auto loadingMaterial = std::make_unique<Material>();
        loadingMaterial->info.name = MATERIAL_NAME;
        auto* loadingMaterialInfo =
            loadingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<Material>>(ASSET_TYPE_MATERIAL, MATERIAL_NAME, loadingMaterial.get()));

        AssetCreatorCollection creatorCollection(loadingZone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext loadingContext(loadingZone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = tracer::CreateRawLoaderIW4(loadingZone.Memory(), loadingSearchPath, loadingZone);
        const auto result = loader->CreateAsset(TRACER_NAME, loadingContext);

        REQUIRE(result.HasBeenSuccessful());
        const auto* loadedAssetInfo = reinterpret_cast<XAssetInfo<TracerDef>*>(result.GetAssetInfo());
        const auto* loadedTracer = loadedAssetInfo->Asset();

        RequireTracerValues(*loadedTracer, *testTracer.tracer);
        REQUIRE(loadedTracer->material == loadingMaterial.get());
        REQUIRE(loadedAssetInfo->m_dependencies.size() == 1u);
        REQUIRE(loadedAssetInfo->m_dependencies[0] == loadingMaterialInfo);
    }
} // namespace
