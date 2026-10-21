#include "Game/IW4/Menu/MenuWriterIW4.h"

#include "Parsing/Menu/MenuFileReader.h"
#include "SearchPath/MockSearchPath.h"

#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <string>
#include <vector>

using namespace IW4;

namespace
{
    expressionEntry Operator(const int operation)
    {
        expressionEntry entry{};
        entry.type = EET_OPERATOR;
        entry.data.op = operation;
        return entry;
    }

    expressionEntry IntOperand(const int value)
    {
        expressionEntry entry{};
        entry.type = EET_OPERAND;
        entry.data.operand.dataType = VAL_INT;
        entry.data.operand.internals.intVal = value;
        return entry;
    }

    TEST_CASE("MenuWriterIW4: Can reload grouped function arguments", "[iw4][menu][assetdumper]")
    {
        std::vector<expressionEntry> entries;
        std::string expectedExpression;

        SECTION("Parentheses around an entire argument list")
        {
            entries = {
                Operator(EXP_FUNC_MAX),
                Operator(OP_LEFTPAREN),
                IntOperand(3),
                Operator(OP_COMMA),
                IntOperand(4),
                Operator(OP_RIGHTPAREN),
                Operator(OP_RIGHTPAREN),
                Operator(OP_DIVIDE),
                IntOperand(2),
            };
            expectedExpression = "max(3, 4) / 2";
        }

        SECTION("Multiple parentheses around an entire argument list")
        {
            entries = {
                Operator(EXP_FUNC_MAX),
                Operator(OP_LEFTPAREN),
                Operator(OP_LEFTPAREN),
                IntOperand(3),
                Operator(OP_COMMA),
                IntOperand(4),
                Operator(OP_RIGHTPAREN),
                Operator(OP_RIGHTPAREN),
                Operator(OP_RIGHTPAREN),
            };
            expectedExpression = "max(3, 4)";
        }

        SECTION("Nested function calls")
        {
            entries = {
                Operator(EXP_FUNC_MAX),
                Operator(OP_LEFTPAREN),
                Operator(EXP_FUNC_MIN),
                Operator(OP_LEFTPAREN),
                IntOperand(3),
                Operator(OP_COMMA),
                IntOperand(4),
                Operator(OP_RIGHTPAREN),
                Operator(OP_RIGHTPAREN),
                Operator(OP_COMMA),
                IntOperand(5),
                Operator(OP_RIGHTPAREN),
                Operator(OP_RIGHTPAREN),
                Operator(OP_DIVIDE),
                IntOperand(2),
            };
            expectedExpression = "max(min(3, 4), 5) / 2";
        }

        SECTION("Grouping inside an argument is preserved")
        {
            entries = {
                Operator(EXP_FUNC_MAX),
                Operator(OP_LEFTPAREN),
                Operator(OP_LEFTPAREN),
                IntOperand(1),
                Operator(OP_ADD),
                IntOperand(2),
                Operator(OP_RIGHTPAREN),
                Operator(OP_MULTIPLY),
                IntOperand(3),
                Operator(OP_COMMA),
                IntOperand(4),
                Operator(OP_RIGHTPAREN),
                Operator(OP_RIGHTPAREN),
            };
            expectedExpression = "max((1 + 2) * 3, 4)";
        }

        SECTION("An argument list without extra parentheses")
        {
            entries = {
                Operator(EXP_FUNC_MAX),
                Operator(OP_LEFTPAREN),
                IntOperand(1),
                Operator(OP_ADD),
                IntOperand(2),
                Operator(OP_RIGHTPAREN),
                Operator(OP_MULTIPLY),
                IntOperand(3),
                Operator(OP_COMMA),
                IntOperand(4),
                Operator(OP_RIGHTPAREN),
            };
            expectedExpression = "max((1 + 2) * 3, 4)";
        }

        Statement_s statement{static_cast<int>(entries.size()), entries.data()};
        menuDef_t menu{};
        menu.window.name = "test";
        menu.visibleExp = &statement;

        std::ostringstream output;
        const auto writer = menu::CreateMenuWriterIW4(output);
        writer->Start();
        writer->WriteMenu(menu);
        writer->End();
        INFO(output.str());
        CHECK(output.str().find("when(" + expectedExpression + ");") != std::string::npos);

        MockSearchPath searchPath;
        menu::MenuAssetZoneState zoneState;
        std::istringstream input(output.str());
        menu::MenuFileReader reader(input, "ui_mp/test.menu", menu::FeatureLevel::IW4, searchPath);
        reader.IncludeZoneState(zoneState);
        const auto parsed = reader.ReadMenuFile();
        REQUIRE(parsed);
        REQUIRE(parsed->m_menus.size() == 1);
        CHECK(parsed->m_menus[0]->m_visible_expression);
    }
} // namespace
