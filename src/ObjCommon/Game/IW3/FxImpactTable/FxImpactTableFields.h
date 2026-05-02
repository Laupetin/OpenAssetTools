#pragma once

#include "Game/IW3/IW3.h"

#include <map>
#include <utility>

namespace IW3
{
    static const char* fx_impact_type_names[] = {"bullet_small_normal",
                                                 "bullet_small_exit",
                                                 "bullet_large_normal",
                                                 "bullet_large_exit",
                                                 "bullet_ap_normal",
                                                 "bullet_ap_exit",
                                                 "shotgun_normal",
                                                 "shotgun_exit",
                                                 "grenade_bounce",
                                                 "grenade_explode",
                                                 "rocket_explode",
                                                 "projectile_dud"};

    static const char* fx_nonflesh_surface_type_names[] = {"default", "bark",   "brick",   "carpet", "cloth",       "concrete", "dirt",    "flesh",
                                                           "foliage", "glass",  "grass",   "gravel", "ice",         "metal",    "mud",     "paper",
                                                           "plaster", "rock",   "sand",    "snow",   "water",       "wood",     "asphalt", "ceramic",
                                                           "plastic", "rubber", "cushion", "fruit",  "paintedmetal"};

    static const char* fx_flesh_surface_type_names[] = {"flesh_body_nonfatal", "flesh_body_fatal", "flesh_head_nonfatal", "flesh_head_fatal"};
} // namespace IW3
