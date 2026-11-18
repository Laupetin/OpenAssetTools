#include "Game/T5/PhysConstraints/RawLoaderPhysConstraintsT5.h"

#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace T5;
using namespace Catch;

namespace
{
    TEST_CASE("PhysConstraints loader restores raw fields (T5)", "[t5][physconstraints][system]")
    {
        constexpr auto RAW_PHYS_CONSTRAINTS = R"(PHYSCONSTRAINTS\c1_type\hinge\c1_bone1_name\tag_hinge\c1_bone2_name\tag_door)"
                                              R"(\c1_offsetX\1\c1_offsetY\2\c1_offsetZ\3\c1_timeout\500)"
                                              R"(\c1_min_health\10\c1_max_health\20\c1_damp\0.25\c1_power\0.5)"
                                              R"(\c1_spin_scale\0.75\c1_min_angle\-45\c1_max_angle\90)"
                                              R"(\c2_type\rope\c2_bone1_name\tag_rope)";

        MockSearchPath loadingSearchPath;
        loadingSearchPath.AddFileData("physconstraints/test_phys_constraints", RAW_PHYS_CONSTRAINTS);

        Zone loadingZone("LoadingZone", 0, GameId::T5, GamePlatform::PC);
        AssetCreatorCollection creatorCollection(loadingZone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext loadingContext(loadingZone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = phys_constraints::CreateRawLoaderT5(loadingZone.Memory(), loadingSearchPath, loadingZone);
        const auto result = loader->CreateAsset("test_phys_constraints", loadingContext);

        REQUIRE(result.HasBeenSuccessful());
        const auto* loadedAssetInfo = reinterpret_cast<XAssetInfo<PhysConstraints>*>(result.GetAssetInfo());
        const auto* loaded = loadedAssetInfo->Asset();

        REQUIRE(std::string(loaded->name) == "test_phys_constraints");
        REQUIRE(loaded->count == 2u);

        const auto& hinge = loaded->data[0];
        REQUIRE(hinge.type == CONSTRAINT_HINGE);
        REQUIRE(std::string(hinge.target_bone1) == "tag_hinge");
        REQUIRE(std::string(hinge.target_bone2) == "tag_door");
        REQUIRE(hinge.attach_point_type1 == ATTACH_POINT_BONE);
        REQUIRE(hinge.attach_point_type2 == ATTACH_POINT_BONE);
        REQUIRE(hinge.offset.x == Approx(1.0f));
        REQUIRE(hinge.offset.y == Approx(2.0f));
        REQUIRE(hinge.offset.z == Approx(3.0f));
        REQUIRE(hinge.timeout == 500);
        REQUIRE(hinge.min_health == 10);
        REQUIRE(hinge.max_health == 20);
        REQUIRE(hinge.damp == Approx(0.25f));
        REQUIRE(hinge.power == Approx(0.5f));
        REQUIRE(hinge.spin_scale == Approx(0.75f));
        REQUIRE(hinge.minAngle == Approx(-45.0f));
        REQUIRE(hinge.maxAngle == Approx(90.0f));

        const auto& rope = loaded->data[1];
        REQUIRE(rope.type == CONSTRAINT_ROPE);
        REQUIRE(std::string(rope.target_bone1) == "tag_rope");
        REQUIRE(std::string(rope.target_bone2).empty());

        REQUIRE(loaded->data[2].type == CONSTRAINT_NONE);
    }
} // namespace
