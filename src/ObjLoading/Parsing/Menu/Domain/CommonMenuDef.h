#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "CommonItemDef.h"
#include "CommonMenuTypes.h"
#include "EventHandler/CommonEventHandlerSet.h"
#include "Expression/ICommonExpression.h"

namespace menu
{
    class CommonMenuDef
    {
    public:
        std::string m_name;
        CommonRect m_rect;
        int m_style;
        int m_border;
        double m_border_size;
        CommonColor m_back_color;
        CommonColor m_fore_color;
        CommonColor m_border_color;
        CommonColor m_focus_color;
        std::string m_background;
        int m_owner_draw;
        int m_owner_draw_flags;
        std::string m_sound_loop;
        double m_fade_clamp;
        int m_fade_cycle;
        double m_fade_amount;
        double m_fade_in_amount;
        double m_blur_radius;
        std::string m_allowed_binding;
        std::unique_ptr<ICommonExpression> m_visible_expression;
        std::unique_ptr<ICommonExpression> m_rect_x_exp;
        std::unique_ptr<ICommonExpression> m_rect_y_exp;
        std::unique_ptr<ICommonExpression> m_rect_w_exp;
        std::unique_ptr<ICommonExpression> m_rect_h_exp;
        std::unique_ptr<ICommonExpression> m_open_sound_exp;
        std::unique_ptr<ICommonExpression> m_close_sound_exp;
        std::unique_ptr<CommonEventHandlerSet> m_on_open;
        std::unique_ptr<CommonEventHandlerSet> m_on_close;
        std::unique_ptr<CommonEventHandlerSet> m_on_request_close;
        std::unique_ptr<CommonEventHandlerSet> m_on_esc;
        std::map<int, std::unique_ptr<CommonEventHandlerSet>> m_key_handler;

        bool m_full_screen;
        bool m_screen_space;
        bool m_decoration;
        bool m_out_of_bounds_click;
        bool m_popup;
        bool m_legacy_split_screen_scale;
        bool m_hidden_during_scope;
        bool m_hidden_during_flashbang;
        bool m_hidden_during_ui;
        bool m_text_only_focus;

        std::vector<std::unique_ptr<CommonItemDef>> m_items;
    };
}
