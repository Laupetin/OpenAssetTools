#include "Game/T5/Destructible/DestructibleDefDumperT5.h"

#include "Game/T5/Destructible/DestructiblePieceFields.h"
#include "Game/T5/ObjConstantsT5.h"
#include "InfoString/InfoString.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <sstream>

using namespace T5;

namespace
{
    InfoString ReadInfoString(const MockOutputPath& output, const std::string& fileName, const std::string& prefix)
    {
        const auto* dumpedFile = output.GetMockedFile(fileName);
        REQUIRE(dumpedFile != nullptr);

        std::istringstream dumpedStream(dumpedFile->AsString());
        InfoString infoString;
        REQUIRE(infoString.FromStream(prefix, dumpedStream));

        return infoString;
    }

    TEST_CASE("DestructibleDef dumper serializes def and pieces (T5)", "[t5][destructibledef][system]")
    {
        Zone dumpingZone("DumpingZone", 0, GameId::T5, GamePlatform::PC);
        const auto doorBone = dumpingZone.m_script_strings.AddOrGetScriptString("tag_door");
        const auto label = dumpingZone.m_script_strings.AddOrGetScriptString("door_label");

        DestructiblePiece pieces[2]{};
        pieces[0].health = 150;
        pieces[0].parentPiece = static_cast<char>(0xFF);
        pieces[0].parentDamagePercent = 0.25f;
        pieces[0].bulletDamageScale = 0.5f;
        pieces[0].explosiveDamageScale = 2.0f;
        pieces[0].meleeDamageScale = 1.0f;
        pieces[0].impactDamageScale = 0.75f;
        pieces[0].entityDamageTransfer = 0.5f;
        pieces[0].damageSound = "dmg_snd";
        pieces[0].enableLabel = label;
        pieces[0].stages[0].showBone = doorBone;
        pieces[0].stages[0].breakHealth = 0.75f;
        pieces[0].stages[0].maxTime = -1.0f;
        pieces[0].stages[0].flags = DSF_EXPLOSIVE_DAMAGE_ONLY | DSF_PHYSICS_PARENT;
        pieces[0].stages[0].breakSound = "break_snd";
        pieces[1].parentPiece = 0;
        pieces[1].stages[1].flags = DSF_MELEE_DAMAGE | DSF_PARENT_DAMAGE;

        DestructibleDef def{};
        def.name = "test_destructible";
        def.numPieces = 2;
        def.pieces = pieces;
        def.clientOnly = 1;
        dumpingZone.m_pools.AddAsset(std::make_unique<XAssetInfo<DestructibleDef>>(ASSET_TYPE_DESTRUCTIBLEDEF,
                                                                                   def.name,
                                                                                   &def,
                                                                                   std::vector<XAssetInfoGeneric*>(),
                                                                                   std::vector{doorBone, label},
                                                                                   std::vector<IndirectAssetReference>(),
                                                                                   &dumpingZone));

        MockSearchPath dumpingObjPath;
        MockOutputPath dumpingOutput;
        AssetDumpingContext dumpingContext(dumpingZone, "", dumpingOutput, dumpingObjPath, std::nullopt);
        destructible::DumperT5 dumper;
        dumper.Dump(dumpingContext);

        const auto defInfo = ReadInfoString(dumpingOutput, "destructibledef/test_destructible", INFO_STRING_PREFIX_DESTRUCTIBLE_DEF);
        REQUIRE(defInfo.GetValueForKey("clientOnly") == "1");
        REQUIRE(defInfo.GetValueForKey("model").empty());
        REQUIRE(defInfo.GetValueForKey("piece0") == "test_destructible_piece0");
        REQUIRE(defInfo.GetValueForKey("piece1") == "test_destructible_piece1");
        REQUIRE(!defInfo.HasKey("piece2"));
        REQUIRE(defInfo.GetValueForKey("piece0Health") == "150");
        REQUIRE(defInfo.GetValueForKey("parentPiece0") == "255");
        REQUIRE(defInfo.GetValueForKey("parentPiece1") == "0");
        REQUIRE(defInfo.GetValueForKey("parentDamage0") == "25");
        REQUIRE(defInfo.GetValueForKey("bulletDamageScale0") == "0.5");
        REQUIRE(defInfo.GetValueForKey("explosiveDamageScale0") == "2");
        REQUIRE(defInfo.GetValueForKey("impactDamageScale0") == "0.75");
        REQUIRE(defInfo.GetValueForKey("damageEntity0") == "50");
        REQUIRE(defInfo.GetValueForKey("damageSound0") == "dmg_snd");
        REQUIRE(defInfo.GetValueForKey("burnSound0").empty());
        REQUIRE(defInfo.GetValueForKey("pieceLabel0") == "door_label");

        const auto pieceInfo = ReadInfoString(dumpingOutput, "destructiblepiece/test_destructible_piece0", INFO_STRING_PREFIX_DESTRUCTIBLE_PIECE);
        REQUIRE(pieceInfo.GetValueForKey("showBone") == "tag_door");
        REQUIRE(pieceInfo.GetValueForKey("breakHealth") == "75");
        REQUIRE(pieceInfo.GetValueForKey("maxTime") == "-1");
        REQUIRE(pieceInfo.GetValueForKey("explosiveDamageOnly") == "1");
        REQUIRE(pieceInfo.GetValueForKey("meleeDamage") == "0");
        REQUIRE(pieceInfo.GetValueForKey("parentDamage") == "0");
        REQUIRE(pieceInfo.GetValueForKey("physicsParent") == "1");
        REQUIRE(pieceInfo.GetValueForKey("breakSound") == "break_snd");
        REQUIRE(pieceInfo.GetValueForKey("loopSound").empty());

        const auto secondPieceInfo = ReadInfoString(dumpingOutput, "destructiblepiece/test_destructible_piece1", INFO_STRING_PREFIX_DESTRUCTIBLE_PIECE);
        REQUIRE(secondPieceInfo.GetValueForKey("meleeDamageD1") == "1");
        REQUIRE(secondPieceInfo.GetValueForKey("parentDamageD1") == "1");
        REQUIRE(secondPieceInfo.GetValueForKey("explosiveDamageOnlyD1") == "0");
    }
} // namespace
