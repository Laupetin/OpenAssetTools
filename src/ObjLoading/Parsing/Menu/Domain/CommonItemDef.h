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

    class CommonItemFeaturesListBox
    {
    public:
        class Column
        {
        public:
            int m_x_pos;
            int m_y_pos;
            int m_width;
            int m_height;
            int m_max_chars;
            int m_alignment;
        };

        bool m_not_selectable;
        bool m_no_scrollbars;
        bool m_use_paging;
        double m_element_width;
        double m_element_height;
        double m_feeder;
        int m_element_style;
        CommonColor m_select_border;
        std::string m_select_icon;

        std::unique_ptr<CommonEventHandlerSet> m_on_double_click;
        std::vector<Column> m_columns;
    };

    class CommonItemFeaturesEditField
    {
    public:
        std::string m_dvar;
        std::string m_local_var;
        double m_def_val;
        double m_min_val;
        double m_max_val;
        int m_max_chars;
        int m_max_paint_chars;
        bool m_max_chars_goto_next;
    };

    class CommonItemFeaturesMultiValue
    {
    public:
        std::vector<std::string> m_step_names;
        std::vector<std::string> m_string_values;
        std::vector<double> m_double_values;
    };

    class CommonItemFeaturesNewsTicker
    {
    public:
        int m_spacing;
        int m_speed;
        int m_news_feed_id;
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
        std::string m_dvar;
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
        std::map<int, std::unique_ptr<CommonEventHandlerSet>> m_key_handlers;

        std::unique_ptr<CommonItemFeaturesListBox> m_list_box_features;
        std::unique_ptr<CommonItemFeaturesEditField> m_edit_field_features;
        std::unique_ptr<CommonItemFeaturesMultiValue> m_multi_value_features;
        std::string m_enum_dvar_name;
        std::unique_ptr<CommonItemFeaturesNewsTicker> m_news_ticker_features;
    };
}
