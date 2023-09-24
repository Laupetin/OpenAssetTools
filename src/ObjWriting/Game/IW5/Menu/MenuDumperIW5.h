#pragma once

#include <string>

#include "Menu/AbstractMenuDumper.h"
#include "Game/IW5/IW5.h"

namespace IW5
{
    class MenuDumper : public AbstractMenuDumper
    {
        static size_t FindStatementClosingParenthesis(const Statement_s* statement, size_t openingParenthesisPosition);

        void WriteStatementOperator(const Statement_s* statement, size_t& currentPos, bool& spaceNext) const;
        void WriteStatementOperandFunction(const Statement_s* statement, size_t currentPos) const;
        void WriteStatementOperand(const Statement_s* statement, size_t& currentPos, bool& spaceNext) const;
        void WriteStatementEntryRange(const Statement_s* statement, size_t startOffset, size_t endOffset) const;
        void WriteStatement(const Statement_s* statement) const;
        void WriteStatementSkipInitialUnnecessaryParenthesis(const Statement_s* statementValue) const;
        void WriteStatementProperty(const std::string& propertyKey, const Statement_s* statementValue, bool isBooleanStatement) const;

        void WriteSetLocalVarData(const std::string& setFunction, const SetLocalVarData* setLocalVarData) const;
        void WriteUnconditionalScript(const char* script) const;
        void WriteMenuEventHandlerSet(const MenuEventHandlerSet* eventHandlerSet);
        void WriteMenuEventHandlerSetProperty(const std::string& propertyKey, const MenuEventHandlerSet* eventHandlerSetValue);

        void WriteRectProperty(const std::string& propertyKey, const rectDef_s& rect) const;
        void WriteMaterialProperty(const std::string& propertyKey, const Material* materialValue) const;
        void WriteSoundAliasProperty(const std::string& propertyKey, const snd_alias_list_t* soundAliasValue) const;
        void WriteDecodeEffectProperty(const std::string& propertyKey, const itemDef_s* item) const;
        void WriteItemKeyHandlerProperty(const ItemKeyHandler* itemKeyHandlerValue);
        void WriteMultiTokenStringProperty(const std::string& propertyKey, const char* value) const;
        void WriteFloatExpressionsProperty(const ItemFloatExpression* floatExpressions, int floatExpressionCount) const;
        void WriteColumnProperty(const std::string& propertyKey, const listBoxDef_s* listBox) const;

        void WriteListBoxProperties(const itemDef_s* item);
        void WriteDvarFloatProperty(const std::string& propertyKey, const itemDef_s* item, const editFieldDef_s* editField) const;
        void WriteEditFieldProperties(const itemDef_s* item) const;
        void WriteMultiValueProperty(const multiDef_s* multiDef) const;
        void WriteMultiProperties(const itemDef_s* item) const;
        void WriteEnumDvarProperties(const itemDef_s* item) const;
        void WriteTickerProperties(const itemDef_s* item) const;

        void WriteItemData(const itemDef_s* item);
        void WriteItemDefs(const itemDef_s* const* itemDefs, size_t itemCount);

        void WriteMenuData(const menuDef_t* menu);

    public:
        explicit MenuDumper(std::ostream& stream);

        void WriteFunctionDef(const std::string& functionName, const Statement_s* statement);
        void WriteMenu(const menuDef_t* menu);
    };
}
