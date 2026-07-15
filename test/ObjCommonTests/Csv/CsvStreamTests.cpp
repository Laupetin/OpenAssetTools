#include "Csv/CsvStream.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <sstream>

namespace
{
#define NEW_LINE "\n"

    TEST_CASE("CsvOutputStream", "[csv]")
    {
        std::ostringstream ss;
        CsvOutputStream outputStream(ss);

        SECTION("Ensure can write normal single-line csv")
        {
            outputStream.WriteColumn("foo");
            outputStream.WriteColumn("bar");

            REQUIRE(ss.str() == "foo,bar");
        }

        SECTION("Ensure can write normal multi-line csv")
        {
            outputStream.WriteColumn("one");
            outputStream.WriteColumn("two");
            outputStream.NextRow();
            outputStream.WriteColumn("foo");
            outputStream.WriteColumn("bar");

            REQUIRE(ss.str() == ("one,two" NEW_LINE "foo,bar"));
        }

        SECTION("Ensure can write csv value with comma")
        {
            outputStream.WriteColumn("first,value");
            outputStream.WriteColumn("two");
            outputStream.NextRow();
            outputStream.WriteColumn("foo");
            outputStream.WriteColumn("second,value");
            outputStream.NextRow();
            outputStream.WriteColumn("crazy");
            outputStream.WriteColumn("comma,value");
            outputStream.WriteColumn("omg");

            REQUIRE(ss.str() == ("\"first,value\",two" NEW_LINE "foo,\"second,value\"" NEW_LINE "crazy,\"comma,value\",omg"));
        }

        SECTION("Ensure can chain multiple values with commas")
        {
            outputStream.WriteColumn("first,value");
            outputStream.WriteColumn("second,value");
            outputStream.WriteColumn("third,value");

            REQUIRE(ss.str() == R"("first,value","second,value","third,value")");
        }

        SECTION("Ensure can write csv value with quotes")
        {
            outputStream.WriteColumn("one");
            outputStream.WriteColumn("two\"value");
            outputStream.WriteColumn("three");

            REQUIRE(ss.str() == R"(one,"two""value",three)");
        }

        SECTION("Ensure can write csv value with newlines")
        {
            outputStream.WriteColumn("one");
            outputStream.WriteColumn("two\nvalue");
            outputStream.WriteColumn("three");

            REQUIRE(ss.str() == ("one,\"two\nvalue\",three"));
        }

        SECTION("Ensure can write csv value with all special cases")
        {
            outputStream.WriteColumn("one");
            outputStream.WriteColumn("two\nvalue,with\"alot\nof,special cases");
            outputStream.WriteColumn("three");

            REQUIRE(ss.str() == ("one,\"two\nvalue,with\"\"alot\nof,special cases\",three"));
        }
    }

    TEST_CASE("CsvInputStream", "[csv]")
    {
        SECTION("Ensure can write normal single-line csv")
        {
            std::istringstream ss("foo,bar");
            CsvInputStream inputStream(ss);

            std::vector<std::string> row;
            inputStream.NextRow(row);

            REQUIRE(row.size() == 2);
            REQUIRE(row[0] == "foo");
            REQUIRE(row[1] == "bar");
        }

        SECTION("Ensure can write normal multi-line csv")
        {
            std::istringstream ss("one,two" NEW_LINE "foo,bar");
            CsvInputStream inputStream(ss);

            std::vector<std::string> row;
            inputStream.NextRow(row);
            REQUIRE(row.size() == 2);
            REQUIRE(row[0] == "one");
            REQUIRE(row[1] == "two");

            inputStream.NextRow(row);
            REQUIRE(row.size() == 2);
            REQUIRE(row[0] == "foo");
            REQUIRE(row[1] == "bar");
        }

        SECTION("Ensure can write csv value with comma")
        {
            std::istringstream ss("\"first,value\",two" NEW_LINE "foo,\"second,value\"" NEW_LINE "crazy,\"comma,value\",omg");
            CsvInputStream inputStream(ss);

            std::vector<std::string> row;
            inputStream.NextRow(row);
            REQUIRE(row.size() == 2);
            REQUIRE(row[0] == "first,value");
            REQUIRE(row[1] == "two");

            inputStream.NextRow(row);
            REQUIRE(row.size() == 2);
            REQUIRE(row[0] == "foo");
            REQUIRE(row[1] == "second,value");

            inputStream.NextRow(row);
            REQUIRE(row.size() == 3);
            REQUIRE(row[0] == "crazy");
            REQUIRE(row[1] == "comma,value");
            REQUIRE(row[2] == "omg");
        }

        SECTION("Ensure can chain multiple values with commas")
        {
            std::istringstream ss(R"("first,value","second,value","third,value")");
            CsvInputStream inputStream(ss);

            std::vector<std::string> row;
            inputStream.NextRow(row);
            REQUIRE(row.size() == 3);
            REQUIRE(row[0] == "first,value");
            REQUIRE(row[1] == "second,value");
            REQUIRE(row[2] == "third,value");
        }

        SECTION("Ensure can write csv value with quotes")
        {
            std::istringstream ss(R"(one,"two""value",three)");
            CsvInputStream inputStream(ss);

            std::vector<std::string> row;
            inputStream.NextRow(row);
            REQUIRE(row.size() == 3);
            REQUIRE(row[0] == "one");
            REQUIRE(row[1] == "two\"value");
            REQUIRE(row[2] == "three");
        }

        SECTION("Ensure can write csv value with newlines")
        {
            std::istringstream ss("one,\"two\nvalue\",three");
            CsvInputStream inputStream(ss);

            std::vector<std::string> row;
            inputStream.NextRow(row);
            REQUIRE(row.size() == 3);
            REQUIRE(row[0] == "one");
            REQUIRE(row[1] == "two\nvalue");
            REQUIRE(row[2] == "three");
        }

        SECTION("Ensure can write csv value with all special cases")
        {
            std::istringstream ss("one,\"two\nvalue,with\"\"alot\nof,special cases\",three");
            CsvInputStream inputStream(ss);

            std::vector<std::string> row;
            inputStream.NextRow(row);
            REQUIRE(row.size() == 3);
            REQUIRE(row[0] == "one");
            REQUIRE(row[1] == "two\nvalue,with\"alot\nof,special cases");
            REQUIRE(row[2] == "three");
        }
    }
} // namespace
