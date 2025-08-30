#include "Image/IPak/IPakCreator.h"

#include "Asset/AssetCreatorCollection.h"
#include "ObjContainer/IPak/IPak.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstring>
#include <filesystem>
#include <memory>

using namespace std::string_literals;

namespace
{
    class TestContext
    {
    public:
        TestContext()
            : m_zone("test", 0, GameId::T6),
              m_zone_states(m_zone),
              m_out_dir()
        {
        }

        image::IPakCreator& CreateSut()
        {
            return m_zone_states.GetZoneAssetCreationState<image::IPakCreator>();
        }

        Zone m_zone;
        MockSearchPath m_search_path;
        ZoneAssetCreationStateContainer m_zone_states;
        MockOutputPath m_out_dir;
    };
} // namespace

namespace test::image::ipak
{
    TEST_CASE("IPakCreator: Does nothing if no ipak was defined", "[image]")
    {
        TestContext testContext;
        auto& sut = testContext.CreateSut();

        sut.Finalize(testContext.m_search_path, testContext.m_out_dir);
        REQUIRE(testContext.m_out_dir.GetMockedFileList().empty());
    }

    TEST_CASE("IPakCreator: Writes IPak file", "[image]")
    {
        TestContext testContext;
        auto& sut = testContext.CreateSut();

        auto* ipak = sut.GetOrAddIPak("amazing");
        ipak->AddImage("random");

        constexpr auto iwiData = "hello world";
        testContext.m_search_path.AddFileData("images/random.iwi", iwiData);

        sut.Finalize(testContext.m_search_path, testContext.m_out_dir);

        const auto* file = testContext.m_out_dir.GetMockedFile("amazing.ipak");
        REQUIRE(file);

        const auto* data = file->m_data.data();
        REQUIRE(data[0] == 'K');
        REQUIRE(data[1] == 'A');
        REQUIRE(data[2] == 'P');
        REQUIRE(data[3] == 'I');

        auto readIpak = IIPak::Create("amazing.ipak", std::make_unique<std::istringstream>(file->AsString()));
        REQUIRE(readIpak->Initialize());

        auto entry = readIpak->GetEntryStream(IIPak::HashString("random"), IIPak::HashData(iwiData, std::char_traits<char>::length(iwiData)));
        REQUIRE(entry);

        char readBuffer[std::char_traits<char>::length(iwiData) + 10];
        entry->read(readBuffer, sizeof(readBuffer));

        REQUIRE(entry->gcount() == std::char_traits<char>::length(iwiData));
        REQUIRE(std::strncmp(iwiData, readBuffer, std::char_traits<char>::length(iwiData)) == 0);
    }
} // namespace test::image::ipak
