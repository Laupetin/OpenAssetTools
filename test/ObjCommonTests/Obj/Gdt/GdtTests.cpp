#include <catch2/catch.hpp>

#include <sstream>

#include "Obj/Gdt/Gdt.h"
#include "Obj/Gdt/GdtStream.h"

namespace obj::gdt
{
	TEST_CASE("Gdt: Ensure can parse simple gdt", "[gdt]")
	{
		std::string gdtString = "{\n"
		"\t\"test_entry\" ( \"test.gdf\" )\n"
		"\t{\n"
		"\t\t\"testkey\" \"testvalue\"\n"
		"\t\t\"test2key\" \"test2value\"\n"
		"\t}\n"
		"}";
		std::istringstream ss(gdtString);

		Gdt gdt;
		GdtReader reader(ss);
		REQUIRE(reader.Read(gdt));

		REQUIRE(gdt.m_entries.size() == 1);

		{
			const auto& entry = *gdt.m_entries[0];
			REQUIRE(entry.m_name == "test_entry");
			REQUIRE(entry.m_gdf_name == "test.gdf");
			REQUIRE(entry.m_parent == nullptr);
			REQUIRE(entry.m_properties.size() == 2);

			REQUIRE(entry.m_properties.at("testkey") == "testvalue");
			REQUIRE(entry.m_properties.at("test2key") == "test2value");
		}
	}

	TEST_CASE("Gdt: Ensure can parse compact gdt", "[gdt]")
	{
		std::string gdtString = "{"
		R"("test_entry"("test.gdf"))"
		"{"
		R"("testkey""testvalue")"
		R"("test2key""test2value")"
		"}"
		"}";
		std::istringstream ss(gdtString);

		Gdt gdt;
		GdtReader reader(ss);
		REQUIRE(reader.Read(gdt));

		REQUIRE(gdt.m_entries.size() == 1);

		{
			const auto& entry = *gdt.m_entries[0];
			REQUIRE(entry.m_name == "test_entry");
			REQUIRE(entry.m_gdf_name == "test.gdf");
			REQUIRE(entry.m_parent == nullptr);
			REQUIRE(entry.m_properties.size() == 2);

			REQUIRE(entry.m_properties.at("testkey") == "testvalue");
			REQUIRE(entry.m_properties.at("test2key") == "test2value");
		}
	}

	TEST_CASE("Gdt: Ensure can parse version section", "[gdt]")
	{
		std::string gdtString = "{"
		R"("version" ( "version.gdf" ))"
		"{"
		R"("game" "t6")"
		R"("version" "1337")"
		"}"
		"}";
		std::istringstream ss(gdtString);

		Gdt gdt;
		GdtReader reader(ss);
		REQUIRE(reader.Read(gdt));

		REQUIRE(gdt.m_entries.empty());
		REQUIRE(gdt.m_version.m_game == "t6");
		REQUIRE(gdt.m_version.m_version == 1337);
	}

	TEST_CASE("Gdt: Ensure can parse version section and entries", "[gdt]")
	{
		std::string gdtString = "{"
		R"("version" ( "version.gdf" ))"
		"{"
		R"("game" "t6")"
		R"("version" "1337")"
		"}"
		R"("test_entry" ( "another_test.gdf" ))"
		"{"
		R"("game" "t6")"
		R"("version" "420")"
		"}"
		"}";
		std::istringstream ss(gdtString);

		Gdt gdt;
		GdtReader reader(ss);
		REQUIRE(reader.Read(gdt));

		REQUIRE(gdt.m_version.m_game == "t6");
		REQUIRE(gdt.m_version.m_version == 1337);

		REQUIRE(gdt.m_entries.size() == 1);

		{
			const auto& entry = *gdt.m_entries[0];
			REQUIRE(entry.m_name == "test_entry");
			REQUIRE(entry.m_gdf_name == "another_test.gdf");
			REQUIRE(entry.m_parent == nullptr);
			REQUIRE(entry.m_properties.size() == 2);

			REQUIRE(entry.m_properties.at("game") == "t6");
			REQUIRE(entry.m_properties.at("version") == "420");
		}
	}

	TEST_CASE("Gdt: Ensure can parse multiple entries", "[gdt]")
	{
		std::string gdtString = "{"
		R"("version" ( "version.gdf" ))"
		"{"
		R"("game" "t6")"
		R"("version" "1337")"
		"}"
		R"("test_entry" ( "another_test.gdf" ))"
		"{"
		R"("game" "t6")"
		R"("version" "420")"
		"}"
		R"("yet_another_entry" ( "super_kewl_asset_type.gdf" ))"
		"{"
		R"("name" "hello")"
		R"("value" "asdf")"
		R"("value2" "22")"
		"}"
		R"("final_entry" ( "quite_boring.gdf" ))"
		"{"
		R"("_HI_" "Hello World")"
		"}"
		"}";
		std::istringstream ss(gdtString);

		Gdt gdt;
		GdtReader reader(ss);
		REQUIRE(reader.Read(gdt));

		REQUIRE(gdt.m_version.m_game == "t6");
		REQUIRE(gdt.m_version.m_version == 1337);

		REQUIRE(gdt.m_entries.size() == 3);

		{
			const auto& entry = *gdt.m_entries[0];
			REQUIRE(entry.m_name == "test_entry");
			REQUIRE(entry.m_gdf_name == "another_test.gdf");
			REQUIRE(entry.m_parent == nullptr);
			REQUIRE(entry.m_properties.size() == 2);

			REQUIRE(entry.m_properties.at("game") == "t6");
			REQUIRE(entry.m_properties.at("version") == "420");
		}

		{
			const auto& entry = *gdt.m_entries[1];
			REQUIRE(entry.m_name == "yet_another_entry");
			REQUIRE(entry.m_gdf_name == "super_kewl_asset_type.gdf");
			REQUIRE(entry.m_parent == nullptr);
			REQUIRE(entry.m_properties.size() == 3);

			REQUIRE(entry.m_properties.at("name") == "hello");
			REQUIRE(entry.m_properties.at("value") == "asdf");
			REQUIRE(entry.m_properties.at("value2") == "22");
		}

		{
			const auto& entry = *gdt.m_entries[2];
			REQUIRE(entry.m_name == "final_entry");
			REQUIRE(entry.m_gdf_name == "quite_boring.gdf");
			REQUIRE(entry.m_parent == nullptr);
			REQUIRE(entry.m_properties.size() == 1);

			REQUIRE(entry.m_properties.at("_HI_") == "Hello World");
		}
	}

	TEST_CASE("Gdt: Ensure can parse entries with parent", "[gdt]")
	{
		std::string gdtString = "{"
		R"("version" ( "version.gdf" ))"
		"{"
		R"("game" "t6")"
		R"("version" "1337")"
		"}"
		R"("test_entry" ( "another_test.gdf" ))"
		"{"
		R"("game" "t6")"
		R"("version" "420")"
		"}"
		R"("yet_another_entry" [ "test_entry" ])"
		"{"
		R"("name" "hello")"
		R"("value" "asdf")"
		R"("value2" "22")"
		"}"
		"}";
		std::istringstream ss(gdtString);

		Gdt gdt;
		GdtReader reader(ss);
		REQUIRE(reader.Read(gdt));

		REQUIRE(gdt.m_version.m_game == "t6");
		REQUIRE(gdt.m_version.m_version == 1337);

		REQUIRE(gdt.m_entries.size() == 2);

		{
			const auto& entry = *gdt.m_entries[0];
			REQUIRE(entry.m_name == "test_entry");
			REQUIRE(entry.m_gdf_name == "another_test.gdf");
			REQUIRE(entry.m_parent == nullptr);
			REQUIRE(entry.m_properties.size() == 2);

			REQUIRE(entry.m_properties.at("game") == "t6");
			REQUIRE(entry.m_properties.at("version") == "420");
		}

		{
			const auto& entry = *gdt.m_entries[1];
			REQUIRE(entry.m_name == "yet_another_entry");
			REQUIRE(entry.m_parent == gdt.m_entries[0].get());
			REQUIRE(entry.m_properties.size() == 3);

			REQUIRE(entry.m_properties.at("name") == "hello");
			REQUIRE(entry.m_properties.at("value") == "asdf");
			REQUIRE(entry.m_properties.at("value2") == "22");
		}
	}

	TEST_CASE("Gdt: Ensure can write simple gdt and parse it again", "[gdt]")
	{
		Gdt gdt;
		gdt.m_version.m_game = "whatagame";
		gdt.m_version.m_version = 6969;

		{
			auto entry = std::make_unique<GdtEntry>("sickentry", "verycool.gdf");
			entry->m_properties["hello"] = "world";
			entry->m_properties["hi"] = "universe";
			gdt.m_entries.emplace_back(std::move(entry));
		}

		{
			auto entry = std::make_unique<GdtEntry>("evencoolerentry", gdt.m_entries[0].get());
			entry->m_properties["nope"] = "xd";
			entry->m_properties["idk"] = "whattotypeanymore";
			gdt.m_entries.emplace_back(std::move(entry));
		}

		std::stringstream ss;
		GdtOutputStream::WriteGdt(gdt, ss);

		std::cout << ss.str();

		Gdt gdt2;
		GdtReader reader(ss);
		REQUIRE(reader.Read(gdt2));

		REQUIRE(gdt2.m_version.m_game == "whatagame");
		REQUIRE(gdt2.m_version.m_version == 6969);

		REQUIRE(gdt2.m_entries.size() == 2);

		{
			const auto& entry = *gdt2.m_entries[0];
			REQUIRE(entry.m_name == "sickentry");
			REQUIRE(entry.m_gdf_name == "verycool.gdf");
			REQUIRE(entry.m_parent == nullptr);
			REQUIRE(entry.m_properties.size() == 2);

			REQUIRE(entry.m_properties.at("hello") == "world");
			REQUIRE(entry.m_properties.at("hi") == "universe");
		}

		{
			const auto& entry = *gdt2.m_entries[1];
			REQUIRE(entry.m_name == "evencoolerentry");
			REQUIRE(entry.m_parent == gdt2.m_entries[0].get());
			REQUIRE(entry.m_properties.size() == 2);

			REQUIRE(entry.m_properties.at("nope") == "xd");
			REQUIRE(entry.m_properties.at("idk") == "whattotypeanymore");
		}
	}
}
