#pragma once

#include <vector>
#include <string>

#include "CommonMenuTypes.h"
#include "EventHandler/CommonEventHandlerSet.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"

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
            int m_x_pos = 0;
            int m_y_pos = 0;
            int m_width = 0;
            int m_height = 0;
            int m_max_chars = 0;
            int m_alignment = 0;
        };

        bool m_not_selectable = false;
        bool m_no_scrollbars = false;
        bool m_use_paging = false;
        double m_element_width = 0;
        double m_element_height = 0;
        double m_feeder = 0;
        int m_element_style = 0;
        CommonColor m_select_border;
        std::string m_select_icon;

        std::unique_ptr<CommonEventHandlerSet> m_on_double_click;
        std::unique_ptr<ISimpleExpression> m_element_height_expression;
        std::vector<Column> m_columns;
    };

    class CommonItemFeaturesEditField
    {
    public:
        std::string m_dvar;
        std::string m_local_var;
        double m_def_val = -1.0;
        double m_min_val = -1.0;
        double m_max_val = -1.0;
        int m_max_chars = 0;
        int m_max_paint_chars = 0;
        bool m_max_chars_goto_next = false;
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
        int m_spacing = 0;
        int m_speed = 0;
        int m_news_feed_id = 0;
    };

    class CommonItemDef
    {
    public:
        class ColorExpressions
        {
        public:
            std::unique_ptr<ISimpleExpression> m_r_exp;
            std::unique_ptr<ISimpleExpression> m_g_exp;
            std::unique_ptr<ISimpleExpression> m_b_exp;
            std::unique_ptr<ISimpleExpression> m_a_exp;
            std::unique_ptr<ISimpleExpression> m_rgb_exp;
        };

        std::string m_name;
        std::string m_text;
        bool m_text_save_game = false;
        bool m_text_cinematic_subtitle = false;
        std::string m_group;
        CommonRect m_rect;
        int m_style = 0;
        bool m_decoration = false;
        bool m_auto_wrapped = false;
        bool m_horizontal_scroll = false;
        int m_type = 0;
        CommonItemFeatureType m_feature_type = CommonItemFeatureType::NONE;
        int m_border = 0;
        double m_border_size = 0;
        int m_owner_draw = 0;
        int m_owner_draw_flags = 0;
        int m_align = 0;
        int m_text_align = 0;
        double m_text_align_x = 0;
        double m_text_align_y = 0;
        double m_text_scale = 0;
        int m_text_style = 0;
        int m_text_font = 0;
        CommonColor m_back_color;
        CommonColor m_fore_color = CommonColor(1.0, 1.0, 1.0, 1.0);
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
        int m_game_message_window_index = 0;
        int m_game_message_window_mode = 0;
        int m_fx_letter_time = 0;
        int m_fx_decay_start_time = 0;
        int m_fx_decay_duration = 0;

        std::unique_ptr<ISimpleExpression> m_visible_expression;
        std::unique_ptr<ISimpleExpression> m_disabled_expression;
        std::unique_ptr<ISimpleExpression> m_text_expression;
        std::unique_ptr<ISimpleExpression> m_text_align_y_expression;
        std::unique_ptr<ISimpleExpression> m_material_expression;
        std::unique_ptr<ISimpleExpression> m_rect_x_exp;
        std::unique_ptr<ISimpleExpression> m_rect_y_exp;
        std::unique_ptr<ISimpleExpression> m_rect_w_exp;
        std::unique_ptr<ISimpleExpression> m_rect_h_exp;
        ColorExpressions m_forecolor_expressions;
        ColorExpressions m_glowcolor_expressions;
        ColorExpressions m_backcolor_expressions;
        std::unique_ptr<CommonEventHandlerSet> m_on_focus;
        std::unique_ptr<CommonEventHandlerSet> m_has_focus;
        std::unique_ptr<CommonEventHandlerSet> m_on_leave_focus;
        std::unique_ptr<CommonEventHandlerSet> m_on_mouse_enter;
        std::unique_ptr<CommonEventHandlerSet> m_on_mouse_exit;
        std::unique_ptr<CommonEventHandlerSet> m_on_mouse_enter_text;
        std::unique_ptr<CommonEventHandlerSet> m_on_mouse_exit_text;
        std::unique_ptr<CommonEventHandlerSet> m_on_action;
        std::unique_ptr<CommonEventHandlerSet> m_on_accept;
        std::multimap<int, std::unique_ptr<CommonEventHandlerSet>> m_key_handlers;

        std::unique_ptr<CommonItemFeaturesListBox> m_list_box_features;
        std::unique_ptr<CommonItemFeaturesEditField> m_edit_field_features;
        std::unique_ptr<CommonItemFeaturesMultiValue> m_multi_value_features;
        std::string m_enum_dvar_name;
        std::unique_ptr<CommonItemFeaturesNewsTicker> m_news_ticker_features;
    };
}
