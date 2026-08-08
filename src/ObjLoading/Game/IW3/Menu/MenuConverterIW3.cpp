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
    constexpr auto WINDOW_FLAG_VISIBLE = 0x4;
    constexpr auto WINDOW_FLAG_NON_DEFAULT_FORECOLOR = 0x00010000;
    constexpr auto WINDOW_FLAG_DECORATION = 0x00100000;
    constexpr auto WINDOW_FLAG_HORIZONTAL_SCROLL = 0x00200000;
    constexpr auto WINDOW_FLAG_AUTO_WRAPPED = 0x00800000;
    constexpr auto WINDOW_FLAG_POPUP = 0x01000000;
    constexpr auto WINDOW_FLAG_OUT_OF_BOUNDS_CLICK = 0x02000000;
    constexpr auto WINDOW_FLAG_LEGACY_SPLIT_SCREEN_SCALE = 0x04000000;
    constexpr auto WINDOW_FLAG_HIDDEN_DURING_FLASH_BANG = 0x10000000;
    constexpr auto WINDOW_FLAG_HIDDEN_DURING_SCOPE = 0x20000000;
    constexpr auto WINDOW_FLAG_HIDDEN_DURING_UI = 0x40000000;

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

        static void ApplyFlag(int& flags, const bool shouldApply, const int flag)
        {
            if (shouldApply)
                flags |= flag;
        }

        [[nodiscard]] Material* ConvertMaterial(const std::string& materialName, const CommonMenuDef* menu, const CommonItemDef* item = nullptr) const
        {
            if (materialName.empty())
                return nullptr;

            auto* dependency = m_context.LoadDependency<AssetMaterial>(materialName);
            if (!dependency)
                throw MenuConversionException(std::format("Failed to load material \"{}\"", materialName), menu, item);

            return dependency->Asset();
        }

        [[nodiscard]] snd_alias_list_t* ConvertSound(const std::string& soundName, const CommonMenuDef* menu, const CommonItemDef* item = nullptr) const
        {
            if (soundName.empty())
                return nullptr;

            auto* dependency = m_context.LoadDependency<AssetSound>(soundName);
            if (!dependency)
                throw MenuConversionException(std::format("Failed to load sound \"{}\"", soundName), menu, item);

            return dependency->Asset();
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

        [[nodiscard]] ItemKeyHandler* ConvertKeyHandlers(const std::multimap<int, std::unique_ptr<CommonEventHandlerSet>>& handlers,
                                                         const CommonMenuDef* menu,
                                                         const CommonItemDef* item = nullptr) const
        {
            if (handlers.empty())
                return nullptr;

            auto* output = m_memory.Alloc<ItemKeyHandler>(handlers.size());
            auto current = handlers.cbegin();
            for (auto i = 0u; i < handlers.size(); i++, ++current)
            {
                output[i].key = current->first;
                output[i].action = ConvertEventHandlerSet(current->second.get(), menu, item);
                output[i].next = i + 1 < handlers.size() ? &output[i + 1] : nullptr;
            }
            return output;
        }

        [[nodiscard]] const char* CreateEnableDvarString(const std::vector<std::string>& elements) const
        {
            std::ostringstream stream;
            for (const auto& element : elements)
                stream << '"' << element << "\" ";
            return m_memory.Dup(stream.str().c_str());
        }

        [[nodiscard]] const char* ConvertEnableDvar(const CommonItemDef& item, int& flags) const
        {
            flags = 0;
            if (!item.m_enable_dvar.empty())
            {
                flags = ITEM_DVAR_FLAG_ENABLE;
                return CreateEnableDvarString(item.m_enable_dvar);
            }
            if (!item.m_disable_dvar.empty())
            {
                flags = ITEM_DVAR_FLAG_DISABLE;
                return CreateEnableDvarString(item.m_disable_dvar);
            }
            if (!item.m_show_dvar.empty())
            {
                flags = ITEM_DVAR_FLAG_SHOW;
                return CreateEnableDvarString(item.m_show_dvar);
            }
            if (!item.m_hide_dvar.empty())
            {
                flags = ITEM_DVAR_FLAG_HIDE;
                return CreateEnableDvarString(item.m_hide_dvar);
            }
            if (!item.m_focus_dvar.empty())
            {
                flags = ITEM_DVAR_FLAG_FOCUS;
                return CreateEnableDvarString(item.m_focus_dvar);
            }
            return nullptr;
        }

        [[nodiscard]] listBoxDef_s* ConvertListBox(const CommonItemDef& commonItem, const CommonMenuDef& menu, itemDef_s& item) const
        {
            const auto* common = commonItem.m_list_box_features.get();
            if (!common)
                return nullptr;

            auto* output = m_memory.Alloc<listBoxDef_s>();
            output->elementWidth = static_cast<float>(common->m_element_width);
            output->elementHeight = static_cast<float>(common->m_element_height);
            output->elementStyle = common->m_element_style;
            output->notselectable = common->m_not_selectable ? 1 : 0;
            output->noScrollBars = common->m_no_scrollbars ? 1 : 0;
            output->usePaging = common->m_use_paging ? 1 : 0;
            output->doubleClick = ConvertEventHandlerSet(common->m_on_double_click.get(), &menu, &commonItem);
            ConvertColor(output->selectBorder, common->m_select_border);
            ConvertColor(output->disableColor, commonItem.m_disable_color);
            output->selectIcon = ConvertMaterial(common->m_select_icon, &menu, &commonItem);
            item.special = static_cast<float>(common->m_feeder);

            output->numColumns = static_cast<int>(std::min(std::size(output->columnInfo), common->m_columns.size()));
            for (auto i = 0; i < output->numColumns; i++)
            {
                output->columnInfo[i].pos = common->m_columns[i].m_x_pos;
                output->columnInfo[i].width = common->m_columns[i].m_width;
                output->columnInfo[i].maxChars = common->m_columns[i].m_max_chars;
                output->columnInfo[i].alignment = common->m_columns[i].m_alignment;
            }
            return output;
        }

        [[nodiscard]] editFieldDef_s* ConvertEditField(const CommonItemDef& commonItem) const
        {
            const auto* common = commonItem.m_edit_field_features.get();
            if (!common)
                return nullptr;

            auto* output = m_memory.Alloc<editFieldDef_s>();
            output->defVal = static_cast<float>(common->m_def_val);
            output->minVal = static_cast<float>(common->m_min_val);
            output->maxVal = static_cast<float>(common->m_max_val);
            output->maxChars = common->m_max_chars;
            output->maxCharsGotoNext = common->m_max_chars_goto_next ? 1 : 0;
            output->maxPaintChars = common->m_max_paint_chars;
            return output;
        }

        [[nodiscard]] multiDef_s* ConvertMultiValue(const CommonItemDef& commonItem) const
        {
            const auto* common = commonItem.m_multi_value_features.get();
            if (!common)
                return nullptr;

            auto* output = m_memory.Alloc<multiDef_s>();
            output->count = static_cast<int>(std::min(std::size(output->dvarList), common->m_step_names.size()));
            output->strDef = common->m_string_values.empty() ? 0 : 1;
            for (auto i = 0; i < output->count; i++)
            {
                output->dvarList[i] = ConvertString(common->m_step_names[i]);
                if (output->strDef && static_cast<unsigned>(i) < common->m_string_values.size())
                    output->dvarStr[i] = ConvertString(common->m_string_values[i]);
                else if (!output->strDef && static_cast<unsigned>(i) < common->m_double_values.size())
                    output->dvarValue[i] = static_cast<float>(common->m_double_values[i]);
            }
            return output;
        }

        [[nodiscard]] itemDef_s* ConvertItem(const CommonMenuDef& menu, const CommonItemDef& common) const
        {
            auto* item = m_memory.Alloc<itemDef_s>();
            item->window.name = ConvertString(common.m_name);
            item->text = common.m_has_text ? m_memory.Dup(common.m_text.c_str()) : nullptr;
            item->window.group = ConvertString(common.m_group);
            item->window.rectClient = ConvertRectDef(common.m_rect);
            item->window.rect = ConvertRectDefRelativeTo(common.m_rect, menu.m_rect);
            item->window.style = common.m_style;
            ApplyFlag(item->window.staticFlags, common.m_decoration, WINDOW_FLAG_DECORATION);
            ApplyFlag(item->window.staticFlags, common.m_auto_wrapped, WINDOW_FLAG_AUTO_WRAPPED);
            ApplyFlag(item->window.staticFlags, common.m_horizontal_scroll, WINDOW_FLAG_HORIZONTAL_SCROLL);
            item->type = common.m_type;
            item->dataType = common.m_type;
            item->window.border = common.m_border;
            item->window.borderSize = static_cast<float>(common.m_border_size);
            ConvertVisibleExpression(item->window, item->visibleExp, common.m_visible_expression.get(), &menu, &common);
            item->window.ownerDraw = common.m_owner_draw;
            item->window.ownerDrawFlags = common.m_owner_draw_flags;
            item->alignment = common.m_align;
            item->textAlignMode = common.m_text_align;
            item->textalignx = static_cast<float>(common.m_text_align_x);
            item->textaligny = static_cast<float>(common.m_text_align_y);
            item->textscale = static_cast<float>(common.m_text_scale);
            item->textStyle = common.m_text_style;
            item->fontEnum = common.m_text_font;
            ConvertColor(item->window.backColor, common.m_back_color);
            ConvertColor(item->window.foreColor, common.m_fore_color);
            ApplyFlag(item->window.dynamicFlags[0], !common.m_fore_color.Equals(CommonColor(1.0, 1.0, 1.0, 1.0)), WINDOW_FLAG_NON_DEFAULT_FORECOLOR);
            ConvertColor(item->window.borderColor, common.m_border_color);
            ConvertColor(item->window.outlineColor, common.m_outline_color);
            item->window.background = ConvertMaterial(common.m_background, &menu, &common);
            item->onFocus = ConvertEventHandlerSet(common.m_on_focus.get(), &menu, &common);
            item->leaveFocus = ConvertEventHandlerSet(common.m_on_leave_focus.get(), &menu, &common);
            item->mouseEnter = ConvertEventHandlerSet(common.m_on_mouse_enter.get(), &menu, &common);
            item->mouseExit = ConvertEventHandlerSet(common.m_on_mouse_exit.get(), &menu, &common);
            item->mouseEnterText = ConvertEventHandlerSet(common.m_on_mouse_enter_text.get(), &menu, &common);
            item->mouseExitText = ConvertEventHandlerSet(common.m_on_mouse_exit_text.get(), &menu, &common);
            item->action = ConvertEventHandlerSet(common.m_on_action.get(), &menu, &common);
            item->onAccept = ConvertEventHandlerSet(common.m_on_accept.get(), &menu, &common);
            item->focusSound = ConvertSound(common.m_focus_sound, &menu, &common);
            item->dvar = ConvertString(common.m_dvar);
            item->dvarTest = ConvertString(common.m_dvar_test);
            item->enableDvar = ConvertEnableDvar(common, item->dvarFlags);
            item->onKey = ConvertKeyHandlers(common.m_key_handlers, &menu, &common);
            ConvertOrApplyStatement(item->text, item->textExp, common.m_text_expression.get(), &menu, &common);
            ConvertOrApplyStatement(item->window.background, item->materialExp, common.m_material_expression.get(), &menu, &common);
            ConvertOrApplyStatement(item->window.rectClient.x, item->rectXExp, common.m_rect_x_exp.get(), &menu, &common);
            ConvertOrApplyStatement(item->window.rectClient.y, item->rectYExp, common.m_rect_y_exp.get(), &menu, &common);
            ConvertOrApplyStatement(item->window.rectClient.w, item->rectWExp, common.m_rect_w_exp.get(), &menu, &common);
            ConvertOrApplyStatement(item->window.rectClient.h, item->rectHExp, common.m_rect_h_exp.get(), &menu, &common);
            ConvertOrApplyStatement(item->window.foreColor[3], item->forecolorAExp, common.m_forecolor_expressions.m_a_exp.get(), &menu, &common);
            item->gameMsgWindowIndex = common.m_game_message_window_index;
            item->gameMsgWindowMode = common.m_game_message_window_mode;

            switch (common.m_feature_type)
            {
            case CommonItemFeatureType::LISTBOX:
                item->typeData.listBox = ConvertListBox(common, menu, *item);
                break;
            case CommonItemFeatureType::EDIT_FIELD:
                item->typeData.editField = ConvertEditField(common);
                break;
            case CommonItemFeatureType::MULTI_VALUE:
                item->typeData.multi = ConvertMultiValue(common);
                break;
            case CommonItemFeatureType::ENUM_DVAR:
                item->typeData.enumDvarName = ConvertString(common.m_enum_dvar_name);
                break;
            default:
                break;
            }

            item->parent = nullptr;
            return item;
        }

        [[nodiscard]] itemDef_s** ConvertItems(const CommonMenuDef& common, menuDef_t& menu) const
        {
            if (common.m_items.empty())
                return nullptr;

            auto* items = m_memory.Alloc<itemDef_s*>(common.m_items.size());
            for (auto i = 0u; i < common.m_items.size(); i++)
            {
                items[i] = ConvertItem(common, *common.m_items[i]);
                items[i]->parent = &menu;
            }
            menu.itemCount = static_cast<int>(common.m_items.size());
            return items;
        }

    public:
        MenuConverter(const bool disableOptimizations, ISearchPath& searchPath, MemoryManager& memory, AssetCreationContext& context)
            : AbstractMenuConverter(disableOptimizations, searchPath, memory, context)
        {
        }

        bool ConvertMenu(const CommonMenuDef& common, menuDef_t& menu, AssetRegistration<AssetMenu>& registration) override
        {
            try
            {
                menu.window.name = m_memory.Dup(common.m_name.c_str());
                menu.fullScreen = common.m_full_screen ? 1 : 0;
                ApplyFlag(menu.window.staticFlags, common.m_decoration, WINDOW_FLAG_DECORATION);
                menu.window.rect = ConvertRectDef(common.m_rect);
                menu.window.rectClient = menu.window.rect;
                menu.window.style = common.m_style;
                menu.window.border = common.m_border;
                menu.window.borderSize = static_cast<float>(common.m_border_size);
                ConvertColor(menu.window.backColor, common.m_back_color);
                ConvertColor(menu.window.foreColor, common.m_fore_color);
                ApplyFlag(menu.window.dynamicFlags[0], !common.m_fore_color.Equals(CommonColor(1.0, 1.0, 1.0, 1.0)), WINDOW_FLAG_NON_DEFAULT_FORECOLOR);
                ConvertColor(menu.window.borderColor, common.m_border_color);
                ConvertColor(menu.window.outlineColor, common.m_outline_color);
                ConvertColor(menu.focusColor, common.m_focus_color);
                ConvertColor(menu.disableColor, common.m_disable_color);
                menu.window.background = ConvertMaterial(common.m_background, &common);
                menu.window.ownerDraw = common.m_owner_draw;
                menu.window.ownerDrawFlags = common.m_owner_draw_flags;
                ApplyFlag(menu.window.staticFlags, common.m_out_of_bounds_click, WINDOW_FLAG_OUT_OF_BOUNDS_CLICK);
                ApplyFlag(menu.window.staticFlags, common.m_popup, WINDOW_FLAG_POPUP);
                ApplyFlag(menu.window.staticFlags, common.m_legacy_split_screen_scale, WINDOW_FLAG_LEGACY_SPLIT_SCREEN_SCALE);
                ApplyFlag(menu.window.staticFlags, common.m_hidden_during_scope, WINDOW_FLAG_HIDDEN_DURING_SCOPE);
                ApplyFlag(menu.window.staticFlags, common.m_hidden_during_flashbang, WINDOW_FLAG_HIDDEN_DURING_FLASH_BANG);
                ApplyFlag(menu.window.staticFlags, common.m_hidden_during_ui, WINDOW_FLAG_HIDDEN_DURING_UI);
                menu.soundName = ConvertString(common.m_sound_loop);
                menu.fadeClamp = static_cast<float>(common.m_fade_clamp);
                menu.fadeCycle = common.m_fade_cycle;
                menu.fadeAmount = static_cast<float>(common.m_fade_amount);
                menu.fadeInAmount = static_cast<float>(common.m_fade_in_amount);
                menu.blurRadius = static_cast<float>(common.m_blur_radius);
                menu.allowedBinding = ConvertString(common.m_allowed_binding);
                ConvertVisibleExpression(menu.window, menu.visibleExp, common.m_visible_expression.get(), &common);
                ConvertOrApplyStatement(menu.window.rect.x, menu.rectXExp, common.m_rect_x_exp.get(), &common);
                ConvertOrApplyStatement(menu.window.rect.y, menu.rectYExp, common.m_rect_y_exp.get(), &common);
                menu.onOpen = ConvertEventHandlerSet(common.m_on_open.get(), &common);
                menu.onClose = ConvertEventHandlerSet(common.m_on_close.get(), &common);
                menu.onESC = ConvertEventHandlerSet(common.m_on_esc.get(), &common);
                menu.onKey = ConvertKeyHandlers(common.m_key_handlers, &common);
                menu.items = ConvertItems(common, menu);
                return true;
            }
            catch (const MenuConversionException& exception)
            {
                PrintConversionExceptionDetails(exception);
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
