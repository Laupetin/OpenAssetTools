#include "MenuConverterIW3.h"

#include "Menu/AbstractMenuConverter.h"
#include "Parsing/Menu/Domain/EventHandler/CommonEventHandlerScript.h"
#include "Parsing/Menu/Domain/Expression/CommonExpressionBaseFunctionCall.h"
#include "Parsing/Menu/Domain/Expression/CommonExpressionCustomFunctionCall.h"
#include "Parsing/Simple/Expression/SimpleExpressionBinaryOperation.h"
#include "Parsing/Simple/Expression/SimpleExpressionConditionalOperator.h"
#include "Parsing/Simple/Expression/SimpleExpressionUnaryOperation.h"

#include <algorithm>
#include <cassert>
#include <format>
#include <sstream>
#include <vector>

using namespace IW3;
using namespace menu;

namespace
{
    class MenuConverter final : public AbstractMenuConverter, public IMenuConverter
    {
        [[nodiscard]] static rectDef_s ConvertRectDef(const CommonRect& rect)
        {
            return rectDef_s{
                static_cast<float>(rect.x),
                static_cast<float>(rect.y),
                static_cast<float>(rect.w),
                static_cast<float>(rect.h),
                rect.horizontalAlign,
                rect.verticalAlign,
            };
        }

        [[nodiscard]] static rectDef_s ConvertRectDefRelativeTo(const CommonRect& rect, const CommonRect& relativeTo)
        {
            return rectDef_s{
                static_cast<float>(relativeTo.x + rect.x),
                static_cast<float>(relativeTo.y + rect.y),
                static_cast<float>(rect.w),
                static_cast<float>(rect.h),
                rect.horizontalAlign,
                rect.verticalAlign,
            };
        }

        static void ConvertColor(float (&output)[4], const CommonColor& input)
        {
            output[0] = static_cast<float>(input.r);
            output[1] = static_cast<float>(input.g);
            output[2] = static_cast<float>(input.b);
            output[3] = static_cast<float>(input.a);
        }

        static void ApplyFlag(int& flags, const bool shouldApply, const int flagValue)
        {
            if (!shouldApply)
                return;

            flags |= flagValue;
        }

        [[nodiscard]] Material* ConvertMaterial(const std::string& materialName, const CommonMenuDef* menu, const CommonItemDef* item = nullptr) const
        {
            if (materialName.empty())
                return nullptr;

            auto* materialDependency = m_context.LoadDependency<AssetMaterial>(materialName);
            if (!materialDependency)
                throw MenuConversionException(std::format("Failed to load material \"{}\"", materialName), menu, item);

            return materialDependency->Asset();
        }

        [[nodiscard]] snd_alias_list_t* ConvertSound(const std::string& soundName, const CommonMenuDef* menu, const CommonItemDef* item = nullptr) const
        {
            if (soundName.empty())
                return nullptr;

            auto* soundDependency = m_context.LoadDependency<AssetSound>(soundName);
            if (!soundDependency)
                throw MenuConversionException(std::format("Failed to load sound \"{}\"", soundName), menu, item);

            return soundDependency->Asset();
        }

        constexpr static operationEnum UNARY_OPERATION_MAPPING[static_cast<unsigned>(SimpleUnaryOperationId::COUNT)]{
            OP_NOT,
            OP_BITWISENOT,
            OP_SUBTRACT,
        };

        constexpr static operationEnum BINARY_OPERATION_MAPPING[static_cast<unsigned>(SimpleBinaryOperationId::COUNT)]{
            OP_ADD,
            OP_SUBTRACT,
            OP_MULTIPLY,
            OP_DIVIDE,
            OP_MODULUS,
            OP_BITWISEAND,
            OP_BITWISEOR,
            OP_BITSHIFTLEFT,
            OP_BITSHIFTRIGHT,
            OP_GREATERTHAN,
            OP_GREATERTHANEQUALTO,
            OP_LESSTHAN,
            OP_LESSTHANEQUALTO,
            OP_EQUALS,
            OP_NOTEQUAL,
            OP_AND,
            OP_OR,
        };

        [[nodiscard]] bool IsOperation(const ISimpleExpression* expression) const
        {
            if (!m_disable_optimizations && expression->IsStatic())
                return false;

            return dynamic_cast<const SimpleExpressionBinaryOperation*>(expression) != nullptr
                   || dynamic_cast<const SimpleExpressionUnaryOperation*>(expression) != nullptr;
        }

        void ConvertExpressionValue(std::vector<expressionEntry>& entries, const SimpleExpressionValue& value) const
        {
            expressionEntry entry{};
            entry.type = EET_OPERAND;

            switch (value.m_type)
            {
            case SimpleExpressionValue::Type::INT:
                entry.data.operand.dataType = VAL_INT;
                entry.data.operand.internals.intVal = value.m_int_value;
                break;
            case SimpleExpressionValue::Type::DOUBLE:
                entry.data.operand.dataType = VAL_FLOAT;
                entry.data.operand.internals.floatVal = static_cast<float>(value.m_double_value);
                break;
            case SimpleExpressionValue::Type::STRING:
                entry.data.operand.dataType = VAL_STRING;
                entry.data.operand.internals.stringVal = m_memory.Dup(value.m_string_value->c_str());
                break;
            }

            entries.emplace_back(entry);
        }

        void ConvertExpressionEntry(std::vector<expressionEntry>& entries,
                                    const ISimpleExpression* expression,
                                    const CommonMenuDef* menu,
                                    const CommonItemDef* item) const
        {
            if (!m_disable_optimizations && expression->IsStatic())
            {
                ConvertExpressionValue(entries, expression->EvaluateStatic());
                return;
            }

            if (const auto* value = dynamic_cast<const SimpleExpressionValue*>(expression))
            {
                ConvertExpressionValue(entries, *value);
                return;
            }

            if (const auto* unary = dynamic_cast<const SimpleExpressionUnaryOperation*>(expression))
            {
                expressionEntry operation{};
                operation.type = EET_OPERATOR;
                operation.data.op = UNARY_OPERATION_MAPPING[static_cast<unsigned>(unary->m_operation_type->m_id)];
                entries.emplace_back(operation);

                const auto wrapOperand = IsOperation(unary->m_operand.get());
                if (wrapOperand)
                    entries.emplace_back(expressionEntry{EET_OPERATOR, {.op = OP_LEFTPAREN}});
                ConvertExpressionEntry(entries, unary->m_operand.get(), menu, item);
                if (wrapOperand)
                    entries.emplace_back(expressionEntry{EET_OPERATOR, {.op = OP_RIGHTPAREN}});
                return;
            }

            if (const auto* binary = dynamic_cast<const SimpleExpressionBinaryOperation*>(expression))
            {
                const auto wrapLeft = IsOperation(binary->m_operand1.get());
                if (wrapLeft)
                    entries.emplace_back(expressionEntry{EET_OPERATOR, {.op = OP_LEFTPAREN}});
                ConvertExpressionEntry(entries, binary->m_operand1.get(), menu, item);
                if (wrapLeft)
                    entries.emplace_back(expressionEntry{EET_OPERATOR, {.op = OP_RIGHTPAREN}});

                expressionEntry operation{};
                operation.type = EET_OPERATOR;
                operation.data.op = BINARY_OPERATION_MAPPING[static_cast<unsigned>(binary->m_operation_type->m_id)];
                entries.emplace_back(operation);

                const auto wrapRight = IsOperation(binary->m_operand2.get());
                if (wrapRight)
                    entries.emplace_back(expressionEntry{EET_OPERATOR, {.op = OP_LEFTPAREN}});
                ConvertExpressionEntry(entries, binary->m_operand2.get(), menu, item);
                if (wrapRight)
                    entries.emplace_back(expressionEntry{EET_OPERATOR, {.op = OP_RIGHTPAREN}});
                return;
            }

            if (const auto* function = dynamic_cast<const CommonExpressionBaseFunctionCall*>(expression))
            {
                expressionEntry functionEntry{};
                functionEntry.type = EET_OPERATOR;
                functionEntry.data.op = static_cast<operationEnum>(function->m_function_index);
                entries.emplace_back(functionEntry);

                auto firstArgument = true;
                for (const auto& argument : function->m_args)
                {
                    if (!firstArgument)
                        entries.emplace_back(expressionEntry{EET_OPERATOR, {.op = OP_COMMA}});
                    firstArgument = false;
                    ConvertExpressionEntry(entries, argument.get(), menu, item);
                }

                entries.emplace_back(expressionEntry{EET_OPERATOR, {.op = OP_RIGHTPAREN}});
                return;
            }

            if (dynamic_cast<const CommonExpressionCustomFunctionCall*>(expression))
                throw MenuConversionException("IW3 does not support custom menu functions", menu, item);
            if (dynamic_cast<const SimpleExpressionConditionalOperator*>(expression))
                throw MenuConversionException("IW3 does not support conditional menu expressions", menu, item);

            throw MenuConversionException("Unknown menu expression entry", menu, item);
        }

        void
            ConvertExpression(statement_s& statement, const ISimpleExpression* expression, const CommonMenuDef* menu, const CommonItemDef* item = nullptr) const
        {
            if (!expression)
                return;

            std::vector<expressionEntry> entries;
            ConvertExpressionEntry(entries, expression, menu, item);

            statement.numEntries = static_cast<int>(entries.size());
            statement.entries = m_memory.Alloc<expressionEntry*>(entries.size());
            for (auto i = 0u; i < entries.size(); i++)
            {
                statement.entries[i] = m_memory.Alloc<expressionEntry>();
                *statement.entries[i] = entries[i];
            }
        }

        void ConvertOrApplyStatement(float& staticValue,
                                     statement_s& statement,
                                     const ISimpleExpression* expression,
                                     const CommonMenuDef* menu,
                                     const CommonItemDef* item = nullptr) const
        {
            if (!expression)
                return;

            if (!m_disable_optimizations && expression->IsStatic())
            {
                const auto value = expression->EvaluateStatic();
                if (value.m_type == SimpleExpressionValue::Type::INT)
                    staticValue = static_cast<float>(value.m_int_value);
                else if (value.m_type == SimpleExpressionValue::Type::DOUBLE)
                    staticValue = static_cast<float>(value.m_double_value);
                else
                    throw MenuConversionException("Cannot convert string expression value to floating point", menu, item);
                return;
            }

            ConvertExpression(statement, expression, menu, item);
        }

        void ConvertOrApplyStatement(const char*& staticValue,
                                     statement_s& statement,
                                     const ISimpleExpression* expression,
                                     const CommonMenuDef* menu,
                                     const CommonItemDef* item = nullptr) const
        {
            if (!expression)
                return;

            if (!m_disable_optimizations && expression->IsStatic())
            {
                const auto value = expression->EvaluateStatic();
                if (value.m_type != SimpleExpressionValue::Type::STRING)
                    throw MenuConversionException("Cannot convert numeric expression value to string", menu, item);
                staticValue = m_memory.Dup(value.m_string_value->c_str());
                return;
            }

            ConvertExpression(statement, expression, menu, item);
        }

        void ConvertOrApplyStatement(Material*& staticValue,
                                     statement_s& statement,
                                     const ISimpleExpression* expression,
                                     const CommonMenuDef* menu,
                                     const CommonItemDef* item = nullptr) const
        {
            if (!expression)
                return;

            if (!m_disable_optimizations && expression->IsStatic())
            {
                const auto value = expression->EvaluateStatic();
                if (value.m_type != SimpleExpressionValue::Type::STRING)
                    throw MenuConversionException("Cannot convert numeric expression value to material", menu, item);
                staticValue = ConvertMaterial(*value.m_string_value, menu, item);
                return;
            }

            ConvertExpression(statement, expression, menu, item);
        }

        void ConvertVisibleExpression(windowDef_t& window,
                                      statement_s& statement,
                                      const ISimpleExpression* expression,
                                      const CommonMenuDef* menu,
                                      const CommonItemDef* item = nullptr) const
        {
            if (!expression)
                return;

            const auto* literal = dynamic_cast<const SimpleExpressionValue*>(expression);
            const auto isStatic = !m_disable_optimizations ? expression->IsStatic() : literal != nullptr;
            if (isStatic)
            {
                const auto value = !m_disable_optimizations ? expression->EvaluateStatic() : *literal;
                if (value.IsTruthy())
                    window.dynamicFlags[0] |= WINDOW_FLAG_VISIBLE;
                return;
            }

            window.dynamicFlags[0] |= WINDOW_FLAG_VISIBLE;
            ConvertExpression(statement, expression, menu, item);
        }

        [[nodiscard]] const char*
            ConvertEventHandlerSet(const CommonEventHandlerSet* handlers, const CommonMenuDef* menu, const CommonItemDef* item = nullptr) const
        {
            if (!handlers)
                return nullptr;

            std::string script;
            for (const auto& element : handlers->m_elements)
            {
                if (element->GetType() != CommonEventHandlerElementType::SCRIPT)
                    throw MenuConversionException("IW3 event handlers must compile to a script string", menu, item);

                script += dynamic_cast<const CommonEventHandlerScript*>(element.get())->m_script;
            }

            return script.empty() ? nullptr : m_memory.Dup(script.c_str());
        }

        [[nodiscard]] ItemKeyHandler* ConvertKeyHandler(const std::multimap<int, std::unique_ptr<CommonEventHandlerSet>>& keyHandlers,
                                                        const CommonMenuDef* menu,
                                                        const CommonItemDef* item = nullptr) const
        {
            if (keyHandlers.empty())
                return nullptr;

            const auto keyHandlerCount = keyHandlers.size();
            auto* output = m_memory.Alloc<ItemKeyHandler>(keyHandlerCount);
            auto currentKeyHandler = keyHandlers.cbegin();
            for (auto i = 0u; i < keyHandlerCount; i++)
            {
                output[i].key = currentKeyHandler->first;
                output[i].action = ConvertEventHandlerSet(currentKeyHandler->second.get(), menu, item);

                if (i + 1 < keyHandlerCount)
                    output[i].next = &output[i + 1];
                else
                    output[i].next = nullptr;
                ++currentKeyHandler;
            }

            return output;
        }

        [[nodiscard]] const char* CreateEnableDvarString(const std::vector<std::string>& stringElements) const
        {
            std::ostringstream ss;

            for (const auto& element : stringElements)
            {
                ss << "\"" << element << "\" ";
            }

            return m_memory.Dup(ss.str().c_str());
        }

        [[nodiscard]] const char* ConvertEnableDvar(const CommonItemDef& commonItem, int& dvarFlags) const
        {
            dvarFlags = 0;

            if (!commonItem.m_enable_dvar.empty())
            {
                dvarFlags |= ITEM_DVAR_FLAG_ENABLE;
                return CreateEnableDvarString(commonItem.m_enable_dvar);
            }

            if (!commonItem.m_disable_dvar.empty())
            {
                dvarFlags |= ITEM_DVAR_FLAG_DISABLE;
                return CreateEnableDvarString(commonItem.m_disable_dvar);
            }

            if (!commonItem.m_show_dvar.empty())
            {
                dvarFlags |= ITEM_DVAR_FLAG_SHOW;
                return CreateEnableDvarString(commonItem.m_show_dvar);
            }

            if (!commonItem.m_hide_dvar.empty())
            {
                dvarFlags |= ITEM_DVAR_FLAG_HIDE;
                return CreateEnableDvarString(commonItem.m_hide_dvar);
            }

            if (!commonItem.m_focus_dvar.empty())
            {
                dvarFlags |= ITEM_DVAR_FLAG_FOCUS;
                return CreateEnableDvarString(commonItem.m_focus_dvar);
            }

            return nullptr;
        }

        [[nodiscard]] listBoxDef_s* ConvertListBoxFeatures(itemDef_s* item,
                                                           CommonItemFeaturesListBox* commonListBox,
                                                           const CommonMenuDef& parentMenu,
                                                           const CommonItemDef& commonItem) const
        {
            if (commonListBox == nullptr)
                return nullptr;

            auto* listBox = m_memory.Alloc<listBoxDef_s>();
            listBox->notselectable = commonListBox->m_not_selectable ? 1 : 0;
            listBox->noScrollBars = commonListBox->m_no_scrollbars ? 1 : 0;
            listBox->usePaging = commonListBox->m_use_paging ? 1 : 0;
            listBox->elementWidth = static_cast<float>(commonListBox->m_element_width);
            listBox->elementHeight = static_cast<float>(commonListBox->m_element_height);
            item->special = static_cast<float>(commonListBox->m_feeder);
            listBox->elementStyle = commonListBox->m_element_style;
            listBox->doubleClick = ConvertEventHandlerSet(commonListBox->m_on_double_click.get(), &parentMenu, &commonItem);
            ConvertColor(listBox->selectBorder, commonListBox->m_select_border);
            ConvertColor(listBox->disableColor, commonItem.m_disable_color);
            listBox->selectIcon = ConvertMaterial(commonListBox->m_select_icon, &parentMenu, &commonItem);

            listBox->numColumns = static_cast<int>(std::min(std::size(listBox->columnInfo), commonListBox->m_columns.size()));
            for (auto i = 0; i < listBox->numColumns; i++)
            {
                auto& col = listBox->columnInfo[i];
                const auto& commonCol = commonListBox->m_columns[i];

                col.pos = commonCol.m_x_pos;
                col.width = commonCol.m_width;
                col.maxChars = commonCol.m_max_chars;
                col.alignment = commonCol.m_alignment;
            }

            return listBox;
        }

        [[nodiscard]] editFieldDef_s* ConvertEditFieldFeatures(CommonItemFeaturesEditField* commonEditField) const
        {
            if (commonEditField == nullptr)
                return nullptr;

            auto* editField = m_memory.Alloc<editFieldDef_s>();
            editField->defVal = static_cast<float>(commonEditField->m_def_val);
            editField->minVal = static_cast<float>(commonEditField->m_min_val);
            editField->maxVal = static_cast<float>(commonEditField->m_max_val);
            editField->maxChars = commonEditField->m_max_chars;
            editField->maxCharsGotoNext = commonEditField->m_max_chars_goto_next ? 1 : 0;
            editField->maxPaintChars = commonEditField->m_max_paint_chars;

            return editField;
        }

        [[nodiscard]] multiDef_s* ConvertMultiValueFeatures(CommonItemFeaturesMultiValue* commonMultiValue) const
        {
            if (commonMultiValue == nullptr)
                return nullptr;

            auto* multiValue = m_memory.Alloc<multiDef_s>();
            multiValue->count = static_cast<int>(std::min(std::size(multiValue->dvarList), commonMultiValue->m_step_names.size()));
            multiValue->strDef = !commonMultiValue->m_string_values.empty() ? 1 : 0;

            for (auto i = 0; i < multiValue->count; i++)
            {
                multiValue->dvarList[i] = ConvertString(commonMultiValue->m_step_names[i]);

                if (multiValue->strDef)
                {
                    if (commonMultiValue->m_string_values.size() > static_cast<unsigned>(i))
                        multiValue->dvarStr[i] = ConvertString(commonMultiValue->m_string_values[i]);
                }
                else
                {
                    if (commonMultiValue->m_double_values.size() > static_cast<unsigned>(i))
                        multiValue->dvarValue[i] = static_cast<float>(commonMultiValue->m_double_values[i]);
                }
            }

            return multiValue;
        }

        [[nodiscard]] itemDef_s* ConvertItem(const CommonMenuDef& commonParentMenu, menuDef_t& parentMenu, const CommonItemDef& commonItem) const
        {
            auto* item = m_memory.Alloc<itemDef_s>();
            item->window.name = ConvertString(commonItem.m_name);
            item->text = commonItem.m_text ? m_memory.Dup(commonItem.m_text->c_str()) : nullptr;
            item->window.group = ConvertString(commonItem.m_group);
            item->window.rectClient = ConvertRectDef(commonItem.m_rect);
            item->window.rect = ConvertRectDefRelativeTo(commonItem.m_rect, commonParentMenu.m_rect);
            item->window.style = commonItem.m_style;
            ApplyFlag(item->window.staticFlags, commonItem.m_decoration, WINDOW_FLAG_DECORATION);
            ApplyFlag(item->window.staticFlags, commonItem.m_auto_wrapped, WINDOW_FLAG_AUTO_WRAPPED);
            ApplyFlag(item->window.staticFlags, commonItem.m_horizontal_scroll, WINDOW_FLAG_HORIZONTAL_SCROLL);
            item->type = commonItem.m_type;
            item->dataType = commonItem.m_type;
            item->window.border = commonItem.m_border;
            item->window.borderSize = static_cast<float>(commonItem.m_border_size);
            ConvertVisibleExpression(item->window, item->visibleExp, commonItem.m_visible_expression.get(), &commonParentMenu, &commonItem);
            item->window.ownerDraw = commonItem.m_owner_draw;
            item->window.ownerDrawFlags = commonItem.m_owner_draw_flags;
            item->alignment = commonItem.m_align;
            item->textAlignMode = commonItem.m_text_align;
            item->textalignx = static_cast<float>(commonItem.m_text_align_x);
            item->textaligny = static_cast<float>(commonItem.m_text_align_y);
            item->textscale = static_cast<float>(commonItem.m_text_scale);
            item->textStyle = commonItem.m_text_style;
            item->fontEnum = commonItem.m_text_font;
            ConvertColor(item->window.backColor, commonItem.m_back_color);
            ConvertColor(item->window.foreColor, commonItem.m_fore_color);
            ApplyFlag(item->window.dynamicFlags[0], !commonItem.m_fore_color.Equals(CommonColor(1.0, 1.0, 1.0, 1.0)), WINDOW_FLAG_NON_DEFAULT_FORECOLOR);
            ConvertColor(item->window.borderColor, commonItem.m_border_color);
            ConvertColor(item->window.outlineColor, commonItem.m_outline_color);
            item->window.background = ConvertMaterial(commonItem.m_background, &commonParentMenu, &commonItem);
            item->onFocus = ConvertEventHandlerSet(commonItem.m_on_focus.get(), &commonParentMenu, &commonItem);
            item->leaveFocus = ConvertEventHandlerSet(commonItem.m_on_leave_focus.get(), &commonParentMenu, &commonItem);
            item->mouseEnter = ConvertEventHandlerSet(commonItem.m_on_mouse_enter.get(), &commonParentMenu, &commonItem);
            item->mouseExit = ConvertEventHandlerSet(commonItem.m_on_mouse_exit.get(), &commonParentMenu, &commonItem);
            item->mouseEnterText = ConvertEventHandlerSet(commonItem.m_on_mouse_enter_text.get(), &commonParentMenu, &commonItem);
            item->mouseExitText = ConvertEventHandlerSet(commonItem.m_on_mouse_exit_text.get(), &commonParentMenu, &commonItem);
            item->action = ConvertEventHandlerSet(commonItem.m_on_action.get(), &commonParentMenu, &commonItem);
            item->onAccept = ConvertEventHandlerSet(commonItem.m_on_accept.get(), &commonParentMenu, &commonItem);
            item->focusSound = ConvertSound(commonItem.m_focus_sound, &commonParentMenu, &commonItem);
            item->dvar = ConvertString(commonItem.m_dvar);
            item->dvarTest = ConvertString(commonItem.m_dvar_test);
            item->enableDvar = ConvertEnableDvar(commonItem, item->dvarFlags);
            item->onKey = ConvertKeyHandler(commonItem.m_key_handlers, &commonParentMenu, &commonItem);
            ConvertOrApplyStatement(item->text, item->textExp, commonItem.m_text_expression.get(), &commonParentMenu, &commonItem);
            ConvertOrApplyStatement(item->window.background, item->materialExp, commonItem.m_material_expression.get(), &commonParentMenu, &commonItem);
            ConvertOrApplyStatement(item->window.rectClient.x, item->rectXExp, commonItem.m_rect_x_exp.get(), &commonParentMenu, &commonItem);
            ConvertOrApplyStatement(item->window.rectClient.y, item->rectYExp, commonItem.m_rect_y_exp.get(), &commonParentMenu, &commonItem);
            ConvertOrApplyStatement(item->window.rectClient.w, item->rectWExp, commonItem.m_rect_w_exp.get(), &commonParentMenu, &commonItem);
            ConvertOrApplyStatement(item->window.rectClient.h, item->rectHExp, commonItem.m_rect_h_exp.get(), &commonParentMenu, &commonItem);
            ConvertOrApplyStatement(
                item->window.foreColor[3], item->forecolorAExp, commonItem.m_forecolor_expressions.m_a_exp.get(), &commonParentMenu, &commonItem);
            item->gameMsgWindowIndex = commonItem.m_game_message_window_index;
            item->gameMsgWindowMode = commonItem.m_game_message_window_mode;

            switch (commonItem.m_feature_type)
            {
            case CommonItemFeatureType::LISTBOX:
                item->typeData.listBox = ConvertListBoxFeatures(item, commonItem.m_list_box_features.get(), commonParentMenu, commonItem);
                break;

            case CommonItemFeatureType::EDIT_FIELD:
                item->typeData.editField = ConvertEditFieldFeatures(commonItem.m_edit_field_features.get());
                break;

            case CommonItemFeatureType::MULTI_VALUE:
                item->typeData.multi = ConvertMultiValueFeatures(commonItem.m_multi_value_features.get());
                break;

            case CommonItemFeatureType::ENUM_DVAR:
                item->typeData.enumDvarName = ConvertString(commonItem.m_enum_dvar_name);
                break;

            case CommonItemFeatureType::NONE:
            default:
                break;
            }

            item->parent = &parentMenu;

            return item;
        }

        itemDef_s** ConvertMenuItems(const CommonMenuDef& commonMenu, menuDef_t& menu, int& itemCount) const
        {
            if (commonMenu.m_items.empty())
            {
                itemCount = 0;
                return nullptr;
            }

            auto* items = m_memory.Alloc<itemDef_s*>(commonMenu.m_items.size());
            for (auto i = 0u; i < commonMenu.m_items.size(); i++)
                items[i] = ConvertItem(commonMenu, menu, *commonMenu.m_items[i]);

            itemCount = static_cast<int>(commonMenu.m_items.size());

            return items;
        }

    public:
        MenuConverter(const bool disableOptimizations, ISearchPath& searchPath, MemoryManager& memory, AssetCreationContext& context)
            : AbstractMenuConverter(disableOptimizations, searchPath, memory, context)
        {
        }

        bool ConvertMenu(const CommonMenuDef& commonMenu, menuDef_t& menu, AssetRegistration<AssetMenu>& registration) override
        {
            try
            {
                menu.window.name = m_memory.Dup(commonMenu.m_name.c_str());
                menu.fullScreen = commonMenu.m_full_screen ? 1 : 0;
                ApplyFlag(menu.window.staticFlags, commonMenu.m_decoration, WINDOW_FLAG_DECORATION);
                menu.window.rect = ConvertRectDef(commonMenu.m_rect);
                menu.window.rectClient = menu.window.rect;
                menu.window.style = commonMenu.m_style;
                menu.window.border = commonMenu.m_border;
                menu.window.borderSize = static_cast<float>(commonMenu.m_border_size);
                ConvertColor(menu.window.backColor, commonMenu.m_back_color);
                ConvertColor(menu.window.foreColor, commonMenu.m_fore_color);
                ApplyFlag(menu.window.dynamicFlags[0], !commonMenu.m_fore_color.Equals(CommonColor(1.0, 1.0, 1.0, 1.0)), WINDOW_FLAG_NON_DEFAULT_FORECOLOR);
                ConvertColor(menu.window.borderColor, commonMenu.m_border_color);
                ConvertColor(menu.window.outlineColor, commonMenu.m_outline_color);
                ConvertColor(menu.focusColor, commonMenu.m_focus_color);
                ConvertColor(menu.disableColor, commonMenu.m_disable_color);
                menu.window.background = ConvertMaterial(commonMenu.m_background, &commonMenu);
                menu.window.ownerDraw = commonMenu.m_owner_draw;
                menu.window.ownerDrawFlags = commonMenu.m_owner_draw_flags;
                ApplyFlag(menu.window.staticFlags, commonMenu.m_out_of_bounds_click, WINDOW_FLAG_OUT_OF_BOUNDS_CLICK);
                menu.soundName = ConvertString(commonMenu.m_sound_loop);
                ApplyFlag(menu.window.staticFlags, commonMenu.m_popup, WINDOW_FLAG_POPUP);
                menu.fadeClamp = static_cast<float>(commonMenu.m_fade_clamp);
                menu.fadeCycle = commonMenu.m_fade_cycle;
                menu.fadeAmount = static_cast<float>(commonMenu.m_fade_amount);
                menu.fadeInAmount = static_cast<float>(commonMenu.m_fade_in_amount);
                menu.blurRadius = static_cast<float>(commonMenu.m_blur_radius);
                ApplyFlag(menu.window.staticFlags, commonMenu.m_legacy_split_screen_scale, WINDOW_FLAG_LEGACY_SPLIT_SCREEN_SCALE);
                ApplyFlag(menu.window.staticFlags, commonMenu.m_hidden_during_scope, WINDOW_FLAG_HIDDEN_DURING_SCOPE);
                ApplyFlag(menu.window.staticFlags, commonMenu.m_hidden_during_flashbang, WINDOW_FLAG_HIDDEN_DURING_FLASH_BANG);
                ApplyFlag(menu.window.staticFlags, commonMenu.m_hidden_during_ui, WINDOW_FLAG_HIDDEN_DURING_UI);
                menu.allowedBinding = ConvertString(commonMenu.m_allowed_binding);
                ConvertVisibleExpression(menu.window, menu.visibleExp, commonMenu.m_visible_expression.get(), &commonMenu);
                ConvertOrApplyStatement(menu.window.rect.x, menu.rectXExp, commonMenu.m_rect_x_exp.get(), &commonMenu);
                ConvertOrApplyStatement(menu.window.rect.y, menu.rectYExp, commonMenu.m_rect_y_exp.get(), &commonMenu);
                menu.onOpen = ConvertEventHandlerSet(commonMenu.m_on_open.get(), &commonMenu);
                menu.onClose = ConvertEventHandlerSet(commonMenu.m_on_close.get(), &commonMenu);
                menu.onESC = ConvertEventHandlerSet(commonMenu.m_on_esc.get(), &commonMenu);
                menu.onKey = ConvertKeyHandler(commonMenu.m_key_handlers, &commonMenu);
                menu.items = ConvertMenuItems(commonMenu, menu, menu.itemCount);
                return true;
            }
            catch (const MenuConversionException& e)
            {
                PrintConversionExceptionDetails(e);
                return false;
            }
        }
    };
} // namespace

std::unique_ptr<IMenuConverter>
    IMenuConverter::Create(const bool disableOptimizations, ISearchPath& searchPath, MemoryManager& memory, AssetCreationContext& context)
{
    return std::make_unique<MenuConverter>(disableOptimizations, searchPath, memory, context);
}
