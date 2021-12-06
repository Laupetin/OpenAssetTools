#include "MenuConverterIW4.h"

#include <cstring>

#include "Menu/AbstractMenuConverter.h"

using namespace IW4;
using namespace menu;

namespace IW4
{
    class MenuConverterImpl : public AbstractMenuConverter
    {
        static void ApplyMenuDefaults(menuDef_t* menu)
        {
            memset(menu, 0, sizeof(menuDef_t));
            menu->window.foreColor[0] = 1.0f;
            menu->window.foreColor[1] = 1.0f;
            menu->window.foreColor[2] = 1.0f;
            menu->window.foreColor[3] = 1.0f;
        }

        static void ApplyItemDefaults(itemDef_s* item)
        {
            memset(item, 0, sizeof(itemDef_s));
            item->window.foreColor[0] = 1.0f;
            item->window.foreColor[1] = 1.0f;
            item->window.foreColor[2] = 1.0f;
            item->window.foreColor[3] = 1.0f;
        }

        _NODISCARD static rectDef_s ConvertRectDef(const CommonRect& rect)
        {
            return rectDef_s{
                static_cast<float>(rect.x),
                static_cast<float>(rect.y),
                static_cast<float>(rect.w),
                static_cast<float>(rect.h),
                static_cast<char>(rect.horizontalAlign),
                static_cast<char>(rect.verticalAlign)
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

        static int ConvertItemType(const int input)
        {
            return input;
        }

        static int ConvertTextFont(const int input)
        {
            return input;
        }

        _NODISCARD Material* ConvertMaterial(const std::string& materialName, const CommonMenuDef* menu, const CommonItemDef* item = nullptr) const
        {
            if (materialName.empty())
                return nullptr;

            auto* materialDependency = m_manager->LoadDependency(ASSET_TYPE_MATERIAL, materialName);
            if (!materialDependency)
                throw MenuConversionException("Failed to load material \"" + materialName + "\"", menu, item);

            return static_cast<Material*>(materialDependency->m_ptr);
        }
        
        _NODISCARD Statement_s* ConvertExpression(const ISimpleExpression* expression) const
        {
            if (!expression)
                return nullptr;

            return nullptr;
        }

        _NODISCARD Statement_s* ConvertOrApplyStatement(float& staticValue, const ISimpleExpression* expression, const CommonMenuDef* menu, const CommonItemDef* item = nullptr) const
        {
            if (m_legacy_mode)
                return ConvertExpression(expression);

            if (!expression)
                return nullptr;

            if (expression->IsStatic())
            {
                const auto value = expression->Evaluate();
                switch (value.m_type)
                {
                case SimpleExpressionValue::Type::DOUBLE:
                    staticValue = static_cast<float>(value.m_double_value);
                    break;
                case SimpleExpressionValue::Type::INT:
                    staticValue = static_cast<float>(value.m_int_value);
                    break;
                case SimpleExpressionValue::Type::STRING:
                    throw MenuConversionException("Cannot convert string expression value to floating point", menu, item);
                }
                return nullptr;
            }

            return ConvertExpression(expression);
        }

        _NODISCARD Statement_s* ConvertOrApplyStatement(const char*& staticValue, const ISimpleExpression* expression, const CommonMenuDef* menu, const CommonItemDef* item = nullptr) const
        {
            if (m_legacy_mode)
                return ConvertExpression(expression);

            if (!expression)
                return nullptr;

            if (expression->IsStatic())
            {
                const auto value = expression->Evaluate();
                switch (value.m_type)
                {
                case SimpleExpressionValue::Type::STRING:
                    staticValue = m_memory->Dup(value.m_string_value->c_str());
                    break;

                case SimpleExpressionValue::Type::DOUBLE:
                case SimpleExpressionValue::Type::INT:
                    throw MenuConversionException("Cannot convert numeric expression value to string", menu, item);
                }
                return nullptr;
            }

            return ConvertExpression(expression);
        }
        _NODISCARD Statement_s* ConvertOrApplyStatement(Material*& staticValue, const ISimpleExpression* expression, const CommonMenuDef* menu, const CommonItemDef* item = nullptr) const
        {
            if (m_legacy_mode)
                return ConvertExpression(expression);

            if (!expression)
                return nullptr;

            if (expression->IsStatic())
            {
                const auto value = expression->Evaluate();
                switch (value.m_type)
                {
                case SimpleExpressionValue::Type::STRING:
                    staticValue = ConvertMaterial(*value.m_string_value, menu, item);
                    break;

                case SimpleExpressionValue::Type::DOUBLE:
                case SimpleExpressionValue::Type::INT:
                    throw MenuConversionException("Cannot convert numeric expression value to string", menu, item);
                }
                return nullptr;
            }

            return ConvertExpression(expression);
        }

        _NODISCARD MenuEventHandlerSet* ConvertEventHandlerSet(const CommonEventHandlerSet* eventHandlerSet) const
        {
            if (!eventHandlerSet)
                return nullptr;

            return nullptr;
        }

        _NODISCARD ItemKeyHandler* ConvertKeyHandler(const std::map<int, std::unique_ptr<CommonEventHandlerSet>>& keyHandlers) const
        {
            if (keyHandlers.empty())
                return nullptr;

            return nullptr;
        }

        _NODISCARD itemDef_s* ConvertItem(const CommonMenuDef& parentMenu, const CommonItemDef& commonItem) const
        {
            auto* item = m_memory->Create<itemDef_s>();
            ApplyItemDefaults(item);

            item->window.name = ConvertString(commonItem.m_name);
            item->text = ConvertString(commonItem.m_text);
            ApplyFlag(item->itemFlags, commonItem.m_text_save_game, ITEM_FLAG_SAVE_GAME_INFO);
            ApplyFlag(item->itemFlags, commonItem.m_text_cinematic_subtitle, ITEM_FLAG_CINEMATIC_SUBTITLE);
            item->window.group = ConvertString(commonItem.m_group);
            item->window.rect = ConvertRectDef(commonItem.m_rect);
            item->window.style = commonItem.m_style;
            ApplyFlag(item->window.staticFlags, commonItem.m_decoration, WINDOW_FLAG_DECORATION);
            ApplyFlag(item->window.staticFlags, commonItem.m_auto_wrapped, WINDOW_FLAG_AUTO_WRAPPED);
            ApplyFlag(item->window.staticFlags, commonItem.m_horizontal_scroll, WINDOW_FLAG_HORIZONTAL_SCROLL);
            item->type = ConvertItemType(commonItem.m_type);
            item->window.border = commonItem.m_border;
            item->window.borderSize = static_cast<float>(commonItem.m_border_size);
            item->visibleExp = ConvertExpression(commonItem.m_visible_expression.get());
            item->disabledExp = ConvertExpression(commonItem.m_disabled_expression.get());
            item->window.ownerDraw = commonItem.m_owner_draw;
            item->window.ownerDrawFlags = commonItem.m_owner_draw_flags;
            item->alignment = commonItem.m_align;
            item->textAlignMode = commonItem.m_text_align;
            item->textalignx = static_cast<float>(commonItem.m_text_align_x);
            item->textaligny = static_cast<float>(commonItem.m_text_align_y);
            item->textscale = static_cast<float>(commonItem.m_text_scale);
            item->textStyle = commonItem.m_text_style;
            item->fontEnum = ConvertTextFont(commonItem.m_text_font);
            ConvertColor(item->window.backColor, commonItem.m_back_color);
            ConvertColor(item->window.foreColor, commonItem.m_fore_color);
            ConvertColor(item->window.borderColor, commonItem.m_border_color);
            ConvertColor(item->window.outlineColor, commonItem.m_outline_color);
            ConvertColor(item->window.disableColor, commonItem.m_disable_color);
            ConvertColor(item->glowColor, commonItem.m_glow_color);
            item->window.background = ConvertMaterial(commonItem.m_background, &parentMenu, &commonItem);
            item->onFocus = ConvertEventHandlerSet(commonItem.m_on_focus.get());
            item->leaveFocus = ConvertEventHandlerSet(commonItem.m_on_leave_focus.get());
            item->mouseEnter = ConvertEventHandlerSet(commonItem.m_on_mouse_enter.get());
            item->mouseExit = ConvertEventHandlerSet(commonItem.m_on_mouse_exit.get());
            item->mouseEnterText = ConvertEventHandlerSet(commonItem.m_on_mouse_enter_text.get());
            item->mouseExitText = ConvertEventHandlerSet(commonItem.m_on_mouse_exit_text.get());
            item->action = ConvertEventHandlerSet(commonItem.m_on_action.get());
            item->accept = ConvertEventHandlerSet(commonItem.m_on_accept.get());
            // item->focusSound
            item->dvarTest = ConvertString(commonItem.m_dvar_test);
            // enableDvar
            item->onKey = ConvertKeyHandler(commonItem.m_key_handlers);
            item->textExp = ConvertOrApplyStatement(item->text, commonItem.m_text_expression.get(), &parentMenu, &commonItem);
            item->materialExp = ConvertOrApplyStatement(item->window.background, commonItem.m_material_expression.get(), &parentMenu, &commonItem);
            item->disabledExp = ConvertExpression(commonItem.m_disabled_expression.get());
            // FloatExpressions
            item->gameMsgWindowIndex = commonItem.m_game_message_window_index;
            item->gameMsgWindowMode = commonItem.m_game_message_window_mode;
            item->fxLetterTime = commonItem.m_fx_letter_time;
            item->fxDecayStartTime = commonItem.m_fx_decay_start_time;
            item->fxDecayDuration = commonItem.m_fx_decay_duration;

            return item;
        }

        _NODISCARD itemDef_s** ConvertMenuItems(const CommonMenuDef& commonMenu) const
        {
            if (commonMenu.m_items.empty())
                return nullptr;

            auto* items = static_cast<itemDef_s**>(m_memory->Alloc(sizeof(void*) * commonMenu.m_items.size()));
            memset(items, 0, sizeof(void*) * commonMenu.m_items.size());

            for(auto i = 0u; i < commonMenu.m_items.size(); i++)
                items[i] = ConvertItem(commonMenu, *commonMenu.m_items[i]);

            return items;
        }

    public:
        MenuConverterImpl(const bool legacyMode, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager)
            : AbstractMenuConverter(legacyMode, searchPath, memory, manager)
        {
        }

        _NODISCARD menuDef_t* ConvertMenu(const CommonMenuDef& commonMenu) const
        {
            auto* menu = m_memory->Create<menuDef_t>();
            ApplyMenuDefaults(menu);

            menu->window.name = m_memory->Dup(commonMenu.m_name.c_str());
            menu->fullScreen = commonMenu.m_full_screen;
            ApplyFlag(menu->window.staticFlags, commonMenu.m_screen_space, WINDOW_FLAG_SCREEN_SPACE);
            ApplyFlag(menu->window.staticFlags, commonMenu.m_decoration, WINDOW_FLAG_DECORATION);
            menu->window.rect = ConvertRectDef(commonMenu.m_rect);
            menu->window.style = commonMenu.m_style;
            menu->window.border = commonMenu.m_border;
            menu->window.borderSize = static_cast<float>(commonMenu.m_border_size);
            ConvertColor(menu->window.backColor, commonMenu.m_back_color);
            ConvertColor(menu->window.foreColor, commonMenu.m_fore_color);
            ConvertColor(menu->window.borderColor, commonMenu.m_border_color);
            ConvertColor(menu->focusColor, commonMenu.m_focus_color);
            menu->window.background = ConvertMaterial(commonMenu.m_background, &commonMenu);
            menu->window.ownerDraw = commonMenu.m_owner_draw;
            menu->window.ownerDrawFlags = commonMenu.m_owner_draw_flags;
            ApplyFlag(menu->window.staticFlags, commonMenu.m_out_of_bounds_click, WINDOW_FLAG_OUT_OF_BOUNDS_CLICK);
            menu->soundName = ConvertString(commonMenu.m_sound_loop);
            ApplyFlag(menu->window.staticFlags, commonMenu.m_popup, WINDOW_FLAG_POPUP);
            menu->fadeClamp = static_cast<float>(commonMenu.m_fade_clamp);
            menu->fadeCycle = commonMenu.m_fade_cycle;
            menu->fadeAmount = static_cast<float>(commonMenu.m_fade_amount);
            menu->fadeInAmount = static_cast<float>(commonMenu.m_fade_in_amount);
            menu->blurRadius = static_cast<float>(commonMenu.m_blur_radius);
            ApplyFlag(menu->window.staticFlags, commonMenu.m_legacy_split_screen_scale, WINDOW_FLAG_LEGACY_SPLIT_SCREEN_SCALE);
            ApplyFlag(menu->window.staticFlags, commonMenu.m_hidden_during_scope, WINDOW_FLAG_HIDDEN_DURING_SCOPE);
            ApplyFlag(menu->window.staticFlags, commonMenu.m_hidden_during_flashbang, WINDOW_FLAG_HIDDEN_DURING_FLASH_BANG);
            ApplyFlag(menu->window.staticFlags, commonMenu.m_hidden_during_ui, WINDOW_FLAG_HIDDEN_DURING_UI);
            menu->allowedBinding = ConvertString(commonMenu.m_allowed_binding);
            ApplyFlag(menu->window.staticFlags, commonMenu.m_text_only_focus, WINDOW_FLAG_TEXT_ONLY_FOCUS);
            menu->visibleExp = ConvertExpression(commonMenu.m_visible_expression.get());
            menu->rectXExp = ConvertOrApplyStatement(menu->window.rect.x, commonMenu.m_rect_x_exp.get(), &commonMenu);
            menu->rectYExp = ConvertOrApplyStatement(menu->window.rect.y, commonMenu.m_rect_y_exp.get(), &commonMenu);
            menu->rectWExp = ConvertOrApplyStatement(menu->window.rect.w, commonMenu.m_rect_w_exp.get(), &commonMenu);
            menu->rectHExp = ConvertOrApplyStatement(menu->window.rect.h, commonMenu.m_rect_h_exp.get(), &commonMenu);
            menu->openSoundExp = ConvertExpression(commonMenu.m_open_sound_exp.get());
            menu->closeSoundExp = ConvertExpression(commonMenu.m_close_sound_exp.get());
            menu->onOpen = ConvertEventHandlerSet(commonMenu.m_on_open.get());
            menu->onClose = ConvertEventHandlerSet(commonMenu.m_on_close.get());
            menu->onCloseRequest = ConvertEventHandlerSet(commonMenu.m_on_request_close.get());
            menu->onESC = ConvertEventHandlerSet(commonMenu.m_on_esc.get());
            menu->onKey = ConvertKeyHandler(commonMenu.m_key_handlers);
            menu->items = ConvertMenuItems(commonMenu);

            return menu;
        }

        std::vector<XAssetInfoGeneric*> m_dependencies;
    };
}

MenuConverter::MenuConverter(const bool legacyMode, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager)
    : m_legacy_mode(legacyMode),
      m_search_path(searchPath),
      m_memory(memory),
      m_manager(manager)
{
}

std::vector<XAssetInfoGeneric*>& MenuConverter::GetDependencies()
{
    return m_dependencies;
}

menuDef_t* MenuConverter::ConvertMenu(const CommonMenuDef& commonMenu)
{
    MenuConverterImpl impl(m_legacy_mode, m_search_path, m_memory, m_manager);

    try
    {
        auto* result = impl.ConvertMenu(commonMenu);
        m_dependencies = std::move(impl.m_dependencies);
        return result;
    }
    catch (const MenuConversionException& e)
    {
        MenuConverterImpl::PrintConversionExceptionDetails(e);
    }

    return nullptr;
}
