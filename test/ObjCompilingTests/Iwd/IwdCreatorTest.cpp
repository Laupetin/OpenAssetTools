#include "Iwd/IwdCreator.h"

#include "Asset/AssetCreatorCollection.h"
#include "Game/T6/T6.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/FileToZlibWrapper.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstring>
#include <filesystem>
#include <memory>
#include <unzip.h>

using namespace T6;
using namespace std::string_literals;

namespace
{
    class TestContext
    {
    public:
        TestContext()
            : m_zone("test", 0, IGame::GetGameById(GameId::T6)),
              m_zone_states(m_zone),
              m_out_dir()
        {
        }

        IwdCreator& CreateSut()
        {
            return m_zone_states.GetZoneAssetCreationState<IwdCreator>();
        }

        Zone m_zone;
        MockSearchPath m_search_path;
        ZoneAssetCreationStateContainer m_zone_states;
        MockOutputPath m_out_dir;
    };
} // namespace

namespace test::iwd
{
    TEST_CASE("IwdCreator: Does nothing if no iwd was defined", "[image]")
    {
        TestContext testContext;
        auto& sut = testContext.CreateSut();

        sut.Finalize(testContext.m_search_path, testContext.m_out_dir);
        REQUIRE(testContext.m_out_dir.GetMockedFileList().empty());
    }

    TEST_CASE("IwdCreator: Writes Iwd file", "[image]")
    {
        TestContext testContext;
        auto& sut = testContext.CreateSut();

        auto* ipak = sut.GetOrAddIwd("amazing");
        ipak->AddFile("images/random.iwi");

        constexpr auto iwiData = "hello world";
        testContext.m_search_path.AddFileData("images/random.iwi", iwiData);

        sut.Finalize(testContext.m_search_path, testContext.m_out_dir);

        const auto* file = testContext.m_out_dir.GetMockedFile("amazing.iwd");
        REQUIRE(file);

        const auto* data = file->m_data.data();
        REQUIRE(data[0] == 'P');
        REQUIRE(data[1] == 'K');

        std::istringstream ss(file->AsString());
        auto zlibFunctions = FileToZlibWrapper::CreateFunctions32ForFile(&ss);
        auto zip = unzOpen2("amazing.iwd", &zlibFunctions);
        REQUIRE(zip);

        REQUIRE(unzGoToFirstFile(zip) == UNZ_OK);

        unz_file_info fileInfo;
        char fileNameBuffer[64];
        char readBuffer[std::char_traits<char>::length(iwiData) + 10];
        REQUIRE(unzGetCurrentFileInfo(zip, &fileInfo, fileNameBuffer, sizeof(fileNameBuffer), nullptr, 0, nullptr, 0) == UNZ_OK);

        REQUIRE("images/random.iwi"s == fileNameBuffer);
        REQUIRE(fileInfo.uncompressed_size == std::char_traits<char>::length(iwiData));

        REQUIRE(unzOpenCurrentFile(zip) == UNZ_OK);
        REQUIRE(unzReadCurrentFile(zip, readBuffer, sizeof(readBuffer)) == std::char_traits<char>::length(iwiData));
        REQUIRE(std::strncmp(iwiData, readBuffer, std::char_traits<char>::length(iwiData)) == 0);
    }
} // namespace test::iwd
