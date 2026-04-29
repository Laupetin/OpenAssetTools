#pragma once

#include "Game/IW3/IW3.h"

namespace IW3
{
    inline cspField_t snd_alias_fields[]{
        {"aliasName",          offsetof(snd_alias_t_info, aliasName),          CSPFT_STRING},
        {"subtitle",           offsetof(snd_alias_t_info, subtitle),           CSPFT_STRING},
        {"secondaryaliasname", offsetof(snd_alias_t_info, secondaryAliasName), CSPFT_STRING},
        {"chainAliasName",     offsetof(snd_alias_t_info, chainAliasName),     CSPFT_STRING},
        {"sequence",           offsetof(snd_alias_t_info, sequence),           CSPFT_INT   },
        {"volMin",             offsetof(snd_alias_t_info, volMin),             CSPFT_FLOAT },
        {"volMax",             offsetof(snd_alias_t_info, volMax),             CSPFT_FLOAT },
        {"pitchMin",           offsetof(snd_alias_t_info, pitchMin),           CSPFT_FLOAT },
        {"pitchMax",           offsetof(snd_alias_t_info, pitchMax),           CSPFT_FLOAT },
        {"distMin",            offsetof(snd_alias_t_info, distMin),            CSPFT_FLOAT },
        {"distMax",            offsetof(snd_alias_t_info, distMax),            CSPFT_FLOAT },
        {"probability",        offsetof(snd_alias_t_info, probability),        CSPFT_FLOAT },
        {"lfePercentage",      offsetof(snd_alias_t_info, lfePercentage),      CSPFT_FLOAT },
        {"centerPercentage",   offsetof(snd_alias_t_info, centerPercentage),   CSPFT_FLOAT },
        {"startDelay",         offsetof(snd_alias_t_info, startDelay),         CSPFT_INT   },
        {"envelopMin",         offsetof(snd_alias_t_info, envelopMin),         CSPFT_FLOAT },
        {"envelopMax",         offsetof(snd_alias_t_info, envelopMax),         CSPFT_FLOAT },
        {"envelopPercentage",  offsetof(snd_alias_t_info, envelopPercentage),  CSPFT_FLOAT },
    };
}
