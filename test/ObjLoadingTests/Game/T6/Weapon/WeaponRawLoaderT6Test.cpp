#include "Game/T6/Weapon/WeaponRawLoaderT6.h"

#include "Game/T6/Weapon/AttachmentRawLoaderT6.h"
#include "Game/T6/Weapon/AttachmentUniqueRawLoaderT6.h"
#include "OatTestPaths.h"
#include "SearchPath/MockSearchPath.h"
#include "TestFiles.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace T6;
using namespace Catch;

TEST_CASE("WeaponRawLoaderT6: Weapon files are properly loaded (T6)", "[t6][weapon]")
{
    MockSearchPath searchPath;

    const auto currentDir = oat::paths::GetTestDirectory() / "ObjLoadingTests/Game/T6/Weapon";
    searchPath.AddFileData("weapons/pdw57_mp", LoadTestFile(currentDir / "pdw57_mp"));
    searchPath.AddFileData("attachment/defaultattachment", LoadTestFile(currentDir / "defaultattachment"));
    searchPath.AddFileData("attachment/silencer_smg", LoadTestFile(currentDir / "silencer_smg"));
    searchPath.AddFileData("attachmentunique/au_pdw57_none", LoadTestFile(currentDir / "au_pdw57_none"));
    searchPath.AddFileData("attachmentunique/au_pdw57_silencer", LoadTestFile(currentDir / "au_pdw57_silencer"));

    Zone zone("MockZone", 0, GameId::T6, GamePlatform::PC);

    MemoryManager memory;
    AssetCreatorCollection creatorCollection(zone);
    IgnoredAssetLookup ignoredAssetLookup;
    AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

    creatorCollection.AddAssetCreator(attachment::CreateRawLoaderT6(memory, searchPath, zone));
    creatorCollection.AddAssetCreator(attachment_unique::CreateRawLoaderT6(memory, searchPath, zone));
    creatorCollection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetFx>>(memory));
    creatorCollection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetMaterial>>(memory));
    creatorCollection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetTracer>>(memory));
    creatorCollection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetWeaponCamo>>(memory));
    creatorCollection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetXModel>>(memory));
    ignoredAssetLookup.Ignore<AssetFx>("weapon/muzzleflashes/fx_muz_smg_flash_1p");
    ignoredAssetLookup.Ignore<AssetFx>("weapon/muzzleflashes/fx_muz_smg_flash_3p");
    ignoredAssetLookup.Ignore<AssetFx>("weapon/shellejects/fx_rifle");
    ignoredAssetLookup.Ignore<AssetFx>("weapon/muzzleflashes/fx_muz_silencer_md");
    ignoredAssetLookup.Ignore<AssetFx>("weapon/muzzleflashes/fx_muz_silencer_md_3p");
    ignoredAssetLookup.Ignore<AssetMaterial>("reticle_side_small");
    ignoredAssetLookup.Ignore<AssetMaterial>("hud_mp_firerate_auto");
    ignoredAssetLookup.Ignore<AssetMaterial>("menu_mp_weapons_ar57");
    ignoredAssetLookup.Ignore<AssetTracer>("smg");
    ignoredAssetLookup.Ignore<AssetTracer>("smg_enemy");
    ignoredAssetLookup.Ignore<AssetWeaponCamo>("camo_pdw57");
    ignoredAssetLookup.Ignore<AssetXModel>("t6_wpn_smg_pdw57_view");
    ignoredAssetLookup.Ignore<AssetXModel>("t6_wpn_smg_pdw57_world");
    ignoredAssetLookup.Ignore<AssetXModel>("t6_attach_mag_pdw57_view");
    ignoredAssetLookup.Ignore<AssetXModel>("t6_attach_mag_pdw57_world");
    ignoredAssetLookup.Ignore<AssetXModel>("t6_attach_smg_silencer1_view");
    ignoredAssetLookup.Ignore<AssetXModel>("t6_attach_smg_silencer1_world");

    auto loader = weapon::CreateRawLoaderT6(memory, searchPath, zone);
    auto result = loader->CreateAsset("pdw57_mp", context);

    REQUIRE(result.HasBeenSuccessful());

    const auto* weaponAsset = reinterpret_cast<XAssetInfo<WeaponVariantDef>*>(result.GetAssetInfo());
    const auto* weapon = weaponAsset->Asset();
    const auto* none = weapon->attachments[ATTACHMENT_NONE];
    const auto* noneAu = weapon->attachmentUniques[ATTACHMENT_NONE];
    const auto* silencer = weapon->attachments[ATTACHMENT_SILENCER];
    const auto* silencerAu = weapon->attachmentUniques[ATTACHMENT_SILENCER];

    REQUIRE(std::string(weapon->szInternalName) == "pdw57_mp");
    CHECK(weapon->iAttachments == (1u << ATTACHMENT_SILENCER));
    REQUIRE(none);
    REQUIRE(noneAu);
    REQUIRE(silencer);
    REQUIRE(silencerAu);

    CHECK(noneAu->effectOverrides == ((1u << ATTACHMENT_OVERRIDE_EFFECT_VIEW_FLASH) | (1u << ATTACHMENT_OVERRIDE_EFFECT_WORLD_FLASH)));
    CHECK(noneAu->soundOverrides == ((1u << ATTACHMENT_OVERRIDE_SOUND_FIRE) | (1u << ATTACHMENT_OVERRIDE_SOUND_FIRE_PLAYER)));
    CHECK(noneAu->effectOverrides == silencerAu->effectOverrides);
    CHECK(noneAu->soundOverrides == silencerAu->soundOverrides);
}
