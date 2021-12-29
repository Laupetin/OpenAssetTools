#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "CommonItemDef.h"
#include "CommonMenuTypes.h"
#include "EventHandler/CommonEventHandlerSet.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"

namespace menu
{
    class CommonMenuDef
    {
    public:
        std::string m_name;
        CommonRect m_rect;
        int m_style = 0;
        int m_border = 0;
        double m_border_size = 0;
        CommonColor m_back_color;
        CommonColor m_fore_color = CommonColor(1.0, 1.0, 1.0, 1.0);
        CommonColor m_border_color;
        CommonColor m_focus_color;
        CommonColor m_outline_color;
        std::string m_background;
        int m_owner_draw = 0;
        int m_owner_draw_flags = 0;
        std::string m_sound_loop;
        double m_fade_clamp = 0;
        int m_fade_cycle = 0;
        double m_fade_amount = 0;
        double m_fade_in_amount = 0;
        double m_blur_radius = 0;
        std::string m_allowed_binding;
        std::unique_ptr<ISimpleExpression> m_visible_expression;
        std::unique_ptr<ISimpleExpression> m_rect_x_exp;
        std::unique_ptr<ISimpleExpression> m_rect_y_exp;
        std::unique_ptr<ISimpleExpression> m_rect_w_exp;
        std::unique_ptr<ISimpleExpression> m_rect_h_exp;
        std::unique_ptr<ISimpleExpression> m_open_sound_exp;
        std::unique_ptr<ISimpleExpression> m_close_sound_exp;
        std::unique_ptr<CommonEventHandlerSet> m_on_open;
        std::unique_ptr<CommonEventHandlerSet> m_on_close;
        std::unique_ptr<CommonEventHandlerSet> m_on_request_close;
        std::unique_ptr<CommonEventHandlerSet> m_on_esc;
        std::map<int, std::unique_ptr<CommonEventHandlerSet>> m_key_handlers;

        bool m_full_screen = false;
        bool m_screen_space = false;
        bool m_decoration = false;
        bool m_out_of_bounds_click = false;
        bool m_popup = false;
        bool m_legacy_split_screen_scale = false;
        bool m_hidden_during_scope = false;
        bool m_hidden_during_flashbang = false;
        bool m_hidden_during_ui = false;
        bool m_text_only_focus = false;

        std::vector<std::unique_ptr<CommonItemDef>> m_items;
    };
}
