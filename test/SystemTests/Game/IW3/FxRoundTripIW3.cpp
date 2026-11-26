#include "Game/IW3/Fx/FxEffectDefLoaderIW3.h"
#include "Game/IW3/Fx/FxEffectDefWriterIW3.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <sstream>

using namespace IW3;

namespace
{
    TEST_CASE("FxEffectDef round trip preserves signed IW3 size and scale graphs", "[iw3][fx][fxgraphroundtrip]")
    {
        const auto sign = GENERATE(-1.0f, 0.0f, 1.0f);
        FxElemVelStateSample velocitySamples[2]{};
        FxElemVisStateSample visualSamples[2]{};
        visualSamples[1].base.size[0] = sign * 2.0f;
        visualSamples[1].amplitude.size[0] = sign;
        visualSamples[1].base.size[1] = sign * 4.0f;
        visualSamples[1].amplitude.size[1] = -sign;
        visualSamples[1].base.scale = sign * 6.0f;
        visualSamples[1].amplitude.scale = -sign * 2.0f;

        FxElemDef element{};
        element.elemType = FX_ELEM_TYPE_CLOUD;
        element.lifeSpanMsec = {1000, 0};
        element.velIntervalCount = 1;
        element.visStateIntervalCount = 1;
        element.velSamples = velocitySamples;
        element.visSamples = visualSamples;
        FxEffectDef effect{.name = "test", .elemDefCountOneShot = 1, .elemDefs = &element};

        std::ostringstream output;
        fx::CreateEffectDefWriterIW3(output)->Write(effect);
        INFO(output.str());
        MockSearchPath searchPath;
        searchPath.AddFileData("fx/test.efx", output.str());
        Zone zone("MockZone", 0, GameId::IW3, GamePlatform::PC);
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = fx::CreateLoaderIW3(zone.Memory(), searchPath);
        const auto result = loader->CreateAsset("test", context);
        REQUIRE(result.HasBeenSuccessful());
        const auto* reloaded = reinterpret_cast<XAssetInfo<FxEffectDef>*>(result.GetAssetInfo())->Asset();
        REQUIRE(reloaded->elemDefCountOneShot == 1);
        const auto& reloadedElement = reloaded->elemDefs[0];
        REQUIRE(reloadedElement.visStateIntervalCount == 1);
        for (auto sampleIndex = 0u; sampleIndex < 2u; sampleIndex++)
        {
            const auto& expected = visualSamples[sampleIndex];
            const auto& actual = reloadedElement.visSamples[sampleIndex];
            for (auto component = 0u; component < 2u; component++)
            {
                CHECK(actual.base.size[component] == Catch::Approx(expected.base.size[component]));
                CHECK(actual.amplitude.size[component] == Catch::Approx(expected.amplitude.size[component]));
            }
            CHECK(actual.base.scale == Catch::Approx(expected.base.scale));
            CHECK(actual.amplitude.scale == Catch::Approx(expected.amplitude.scale));
        }
    }
} // namespace
