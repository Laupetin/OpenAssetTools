#pragma once

#include <vector>
#include <string>

#include "CommonMenuTypes.h"
#include "EventHandler/CommonEventHandlerSet.h"
#include "Expression/ICommonExpression.h"

namespace menu
{
    enum class CommonItemFeatureType
    {
        NONE,
        LISTBOX,
        EDIT_FIELD,
        MULTI_VALUE,
        ENUM_DVAR,
        NEWS_TICKER
    };

    class CommonItemDef
    {
    public:
        class ColorExpressions
        {
        public:
            std::unique_ptr<ICommonExpression> m_r_exp;
            std::unique_ptr<ICommonExpression> m_g_exp;
            std::unique_ptr<ICommonExpression> m_b_exp;
            std::unique_ptr<ICommonExpression> m_a_exp;
            std::unique_ptr<ICommonExpression> m_rgb_exp;
        };

        std::string m_name;
        std::string m_text;
        bool m_text_save_game;
        bool m_text_cinematic_subtitle;
        std::string m_group;
        CommonRect m_rect;
        int m_style;
        bool m_decoration;
        bool m_auto_wrapped;
        bool m_horizontal_scroll;
        int m_type;
        CommonItemFeatureType m_feature_type;
        int m_border;
        double m_border_size;
        int m_owner_draw;
        int m_owner_draw_flags;
        int m_align;
        int m_text_align;
        double m_text_align_x;
        double m_text_align_y;
        double m_text_scale;
        int m_text_style;
        int m_text_font;
        CommonColor m_back_color;
        CommonColor m_fore_color;
        CommonColor m_border_color;
        CommonColor m_outline_color;
        CommonColor m_disable_color;
        CommonColor m_glow_color;
        std::string m_background;
        std::string m_focus_sound;
        std::string m_dvar_test;
        std::vector<std::string> m_enable_dvar;
        std::vector<std::string> m_disable_dvar;
        std::vector<std::string> m_show_dvar;
        std::vector<std::string> m_hide_dvar;
        std::vector<std::string> m_focus_dvar;
        int m_game_message_window_index;
        int m_game_message_window_mode;
        int m_fx_letter_time;
        int m_fx_decay_start_time;
        int m_fx_decay_duration;

        std::unique_ptr<ICommonExpression> m_visible_expression;
        std::unique_ptr<ICommonExpression> m_disabled_expression;
        std::unique_ptr<ICommonExpression> m_text_expression;
        std::unique_ptr<ICommonExpression> m_material_expression;
        std::unique_ptr<ICommonExpression> m_rect_x_exp;
        std::unique_ptr<ICommonExpression> m_rect_y_exp;
        std::unique_ptr<ICommonExpression> m_rect_w_exp;
        std::unique_ptr<ICommonExpression> m_rect_h_exp;
        ColorExpressions m_forecolor_expressions;
        ColorExpressions m_glowcolor_expressions;
        ColorExpressions m_backcolor_expressions;
        std::unique_ptr<CommonEventHandlerSet> m_on_focus;
        std::unique_ptr<CommonEventHandlerSet> m_on_leave_focus;
        std::unique_ptr<CommonEventHandlerSet> m_on_mouse_enter;
        std::unique_ptr<CommonEventHandlerSet> m_on_mouse_exit;
        std::unique_ptr<CommonEventHandlerSet> m_on_mouse_enter_text;
        std::unique_ptr<CommonEventHandlerSet> m_on_mouse_exit_text;
        std::unique_ptr<CommonEventHandlerSet> m_on_action;
        std::unique_ptr<CommonEventHandlerSet> m_on_accept;
    };
}
