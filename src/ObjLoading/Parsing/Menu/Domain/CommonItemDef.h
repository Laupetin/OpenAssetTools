#pragma once

#include <string>

#include "CommonMenuTypes.h"

namespace menu
{
    class CommonItemDef
    {
    public:
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
        int m_game_message_window_index;
        int m_game_message_window_mode;
        int m_fx_letter_time;
        int m_fx_decay_start_time;
        int m_fx_decay_duration;
    };
}
