#include "Game/T5/Destructible/RawLoaderDestructibleDefT5.h"

#include "Game/T5/Destructible/DestructiblePieceFields.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace T5;
using namespace Catch;

namespace
{
    TEST_CASE("DestructibleDef loader restores def and pieces (T5)", "[t5][destructibledef][system]")
    {
        constexpr auto RAW_DEF = R"(DESTRUCTIBLEDEF\clientOnly\1\piece0\door\piece0Health\150\parentPiece0\255)"
                                 R"(\parentDamage0\25\bulletDamageScale0\0.5\explosiveDamageScale0\2\meleeDamageScale0\1)"
                                 R"(\impactDamageScale0\0.75\damageEntity0\50\damageSound0\dmg_snd\burnSound0\)"
                                 R"(\pieceLabel0\Door_Label\piece1\\piece1Health\20)";
        constexpr auto RAW_PIECE = R"(DESTRUCTIBLEPIECE\showBone\tag_door\breakHealth\75\maxTime\-1)"
                                   R"(\explosiveDamageOnly\1\meleeDamage\0\parentDamage\0\physicsParent\1)"
                                   R"(\breakSound\break_snd\loopSound\\meleeDamageD1\1\parentDamageD1\1)";

        MockSearchPath loadingSearchPath;
        loadingSearchPath.AddFileData("destructibledef/test_destructible", RAW_DEF);
        loadingSearchPath.AddFileData("destructiblepiece/door", RAW_PIECE);

        Zone loadingZone("LoadingZone", 0, GameId::T5, GamePlatform::PC);
        AssetCreatorCollection creatorCollection(loadingZone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext loadingContext(loadingZone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = destructible::CreateRawLoaderT5(loadingZone.Memory(), loadingSearchPath, loadingZone);
        const auto result = loader->CreateAsset("test_destructible", loadingContext);

        REQUIRE(result.HasBeenSuccessful());
        const auto* def = reinterpret_cast<XAssetInfo<DestructibleDef>*>(result.GetAssetInfo())->Asset();

        REQUIRE(std::string(def->name) == "test_destructible");
        REQUIRE(def->clientOnly == 1);
        REQUIRE(def->model == nullptr);
        REQUIRE(def->numPieces == 2);

        const auto& door = def->pieces[0];
        REQUIRE(door.health == 150);
        REQUIRE(static_cast<unsigned char>(door.parentPiece) == 0xFFu);
        REQUIRE(door.parentDamagePercent == Approx(0.25f));
        REQUIRE(door.bulletDamageScale == Approx(0.5f));
        REQUIRE(door.explosiveDamageScale == Approx(2.0f));
        REQUIRE(door.meleeDamageScale == Approx(1.0f));
        REQUIRE(door.impactDamageScale == Approx(0.75f));
        REQUIRE(door.entityDamageTransfer == Approx(0.5f));
        REQUIRE(std::string(door.damageSound) == "dmg_snd");
        REQUIRE(door.burnSound == nullptr);
        REQUIRE(loadingZone.m_script_strings[door.enableLabel] == "door_label");

        const auto& stage = door.stages[0];
        REQUIRE(loadingZone.m_script_strings[stage.showBone] == "tag_door");
        REQUIRE(stage.breakHealth == Approx(0.75f));
        REQUIRE(stage.maxTime == Approx(-1.0f));
        REQUIRE(stage.flags == (DSF_EXPLOSIVE_DAMAGE_ONLY | DSF_PHYSICS_PARENT));
        REQUIRE(std::string(stage.breakSound) == "break_snd");
        REQUIRE(stage.loopSound == nullptr);
        REQUIRE(stage.breakNotify == nullptr);
        REQUIRE(door.stages[1].flags == (DSF_MELEE_DAMAGE | DSF_PARENT_DAMAGE));

        const auto& empty = def->pieces[1];
        REQUIRE(empty.health == 20);
        REQUIRE(empty.stages[0].flags == 0u);
    }
} // namespace
