#include "Game/T4/Weapon/WeaponRawLoaderT4.h"

#include "Game/T4/T4.h"
#include "OatTestPaths.h"
#include "SearchPath/IWD.h"
#include "Utils/FileToZlibWrapper.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>
#include <zip.h>

using namespace T4;

namespace fs = std::filesystem;

namespace
{
    using IwdEntry = std::pair<std::string_view, std::string_view>;

    struct LoadedWeapon
    {
        std::string m_name;
        std::string m_internal_name;
        std::string m_display_name;
    };

    void WriteIwd(std::ostream& stream, const std::initializer_list<IwdEntry> entries)
    {
        auto zlibFunctions = FileToZlibWrapper::CreateFunctions32ForFile(&stream);
        const auto zipHandle = zipOpen2("weapon_test.iwd", 0, nullptr, &zlibFunctions);

        for (const auto& [entryName, contents] : entries)
        {
            const std::string entryNameString(entryName);
            zipOpenNewFileInZip2(zipHandle, entryNameString.c_str(), nullptr, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0);
            zipWriteInFileInZip(zipHandle, contents.data(), static_cast<unsigned>(contents.size()));
            zipCloseFileInZip(zipHandle);
        }

        zipClose(zipHandle, nullptr);
    }

    LoadedWeapon LoadWeapon(const std::initializer_list<IwdEntry> entries, const std::string& requestedAssetName)
    {
        const auto tempDirectory = oat::paths::GetTempDirectory("WeaponRawLoaderT4");
        fs::create_directories(tempDirectory);
        const auto iwdPath = tempDirectory / "weapon_test.iwd";

        {
            std::ofstream stream(iwdPath, std::ios::out | std::ios::binary);
            WriteIwd(stream, entries);
        }

        const auto searchPath = iwd::LoadFromFile(iwdPath.string());
        REQUIRE(searchPath);

        Zone zone("test", 0, GameId::T4, GamePlatform::PC);
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);
        const auto loader = weapon::CreateRawLoaderT4(zone.Memory(), *searchPath, zone);
        const auto result = loader->CreateAsset(requestedAssetName, context);

        REQUIRE(result.HasBeenSuccessful());
        const auto* loadedAsset = reinterpret_cast<XAssetInfo<WeaponDef>*>(result.GetAssetInfo());
        const auto* weapon = loadedAsset->Asset();
        return {
            loadedAsset->m_name,
            weapon->szInternalName,
            weapon->szDisplayName,
        };
    }

    TEST_CASE("T4 weapon raw loader uses an explicit stock directory without changing the asset name", "[t4][weapon][iwd]")
    {
        const std::initializer_list<IwdEntry> entries{
            {"weapons/mp/test_weapon", R"(WEAPONFILE\displayName\MP weapon)"},
        };
        const auto loadedWeapon = LoadWeapon(entries, "mp/test_weapon");

        REQUIRE(loadedWeapon.m_name == "test_weapon");
        REQUIRE(loadedWeapon.m_internal_name == "test_weapon");
        REQUIRE(loadedWeapon.m_display_name == "MP weapon");
    }

    TEST_CASE("T4 weapon raw loader finds a nested IWD entry for a bare asset name", "[t4][weapon][iwd]")
    {
        const std::initializer_list<IwdEntry> entries{
            {"weapons/sp/test_weapon", R"(WEAPONFILE\displayName\SP weapon)"},
        };
        const auto loadedWeapon = LoadWeapon(entries, "test_weapon");

        REQUIRE(loadedWeapon.m_name == "test_weapon");
        REQUIRE(loadedWeapon.m_internal_name == "test_weapon");
        REQUIRE(loadedWeapon.m_display_name == "SP weapon");
    }

    TEST_CASE("T4 weapon raw loader preserves candidate order during case-insensitive IWD lookup", "[t4][weapon][iwd]")
    {
        const std::initializer_list<IwdEntry> entries{
            {"weapons/mp/Test_Weapon", R"(WEAPONFILE\displayName\MP weapon)"},
            {"weapons/sp/Test_Weapon", R"(WEAPONFILE\displayName\SP weapon)"},
        };
        const auto loadedWeapon = LoadWeapon(entries, "test_weapon");

        REQUIRE(loadedWeapon.m_display_name == "SP weapon");
    }
} // namespace
