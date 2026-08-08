#pragma once
#include "Game/T5/T5.h"

namespace T5
{
    inline cspField_t destructible_piece_fields[]{
        {"showBone",      offsetof(DestructiblePiece, stages[0].showBone),      CSPFT_SCRIPT_STRING},
        {"breakHealth",   offsetof(DestructiblePiece, stages[0].breakHealth),   CSPFT_FLOAT        },
        {"maxTime",       offsetof(DestructiblePiece, stages[0].maxTime),       CSPFT_FLOAT        },

        {"breakEffect",   offsetof(DestructiblePiece, stages[0].breakEffect),   CSPFT_FX           },
        {"breakSound",    offsetof(DestructiblePiece, stages[0].breakSound),    CSPFT_STRING       },
        {"breakNotify",   offsetof(DestructiblePiece, stages[0].breakNotify),   CSPFT_STRING       },
        {"loopSound",     offsetof(DestructiblePiece, stages[0].loopSound),     CSPFT_STRING       },

        {"spawnModel1",   offsetof(DestructiblePiece, stages[0].spawnModel[0]), CSPFT_XMODEL       },
        {"spawnModel2",   offsetof(DestructiblePiece, stages[0].spawnModel[1]), CSPFT_XMODEL       },
        {"spawnModel3",   offsetof(DestructiblePiece, stages[0].spawnModel[2]), CSPFT_XMODEL       },

        {"physPreset",    offsetof(DestructiblePiece, stages[0].physPreset),    CSPFT_PHYS_PRESET  },

        {"showBoneD1",    offsetof(DestructiblePiece, stages[1].showBone),      CSPFT_SCRIPT_STRING},
        {"breakHealthD1", offsetof(DestructiblePiece, stages[1].breakHealth),   CSPFT_FLOAT        },
        {"maxTimeD1",     offsetof(DestructiblePiece, stages[1].maxTime),       CSPFT_FLOAT        },
        {"breakEffectD1", offsetof(DestructiblePiece, stages[1].breakEffect),   CSPFT_FX           },
        {"breakSoundD1",  offsetof(DestructiblePiece, stages[1].breakSound),    CSPFT_STRING       },
        {"breakNotifyD1", offsetof(DestructiblePiece, stages[1].breakNotify),   CSPFT_STRING       },
        {"loopSoundD1",   offsetof(DestructiblePiece, stages[1].loopSound),     CSPFT_STRING       },
        {"spawnModel1D1", offsetof(DestructiblePiece, stages[1].spawnModel[0]), CSPFT_XMODEL       },
        {"spawnModel2D1", offsetof(DestructiblePiece, stages[1].spawnModel[1]), CSPFT_XMODEL       },
        {"spawnModel3D1", offsetof(DestructiblePiece, stages[1].spawnModel[2]), CSPFT_XMODEL       },
        {"physPresetD1",  offsetof(DestructiblePiece, stages[1].physPreset),    CSPFT_PHYS_PRESET  },

        {"showBoneD2",    offsetof(DestructiblePiece, stages[2].showBone),      CSPFT_SCRIPT_STRING},
        {"breakHealthD2", offsetof(DestructiblePiece, stages[2].breakHealth),   CSPFT_FLOAT        },
        {"maxTimeD2",     offsetof(DestructiblePiece, stages[2].maxTime),       CSPFT_FLOAT        },
        {"breakEffectD2", offsetof(DestructiblePiece, stages[2].breakEffect),   CSPFT_FX           },
        {"breakSoundD2",  offsetof(DestructiblePiece, stages[2].breakSound),    CSPFT_STRING       },
        {"breakNotifyD2", offsetof(DestructiblePiece, stages[2].breakNotify),   CSPFT_STRING       },
        {"loopSoundD2",   offsetof(DestructiblePiece, stages[2].loopSound),     CSPFT_STRING       },
        {"spawnModel1D2", offsetof(DestructiblePiece, stages[2].spawnModel[0]), CSPFT_XMODEL       },
        {"spawnModel2D2", offsetof(DestructiblePiece, stages[2].spawnModel[1]), CSPFT_XMODEL       },
        {"spawnModel3D2", offsetof(DestructiblePiece, stages[2].spawnModel[2]), CSPFT_XMODEL       },
        {"physPresetD2",  offsetof(DestructiblePiece, stages[2].physPreset),    CSPFT_PHYS_PRESET  },

        {"showBoneD3",    offsetof(DestructiblePiece, stages[3].showBone),      CSPFT_SCRIPT_STRING},
        {"breakHealthD3", offsetof(DestructiblePiece, stages[3].breakHealth),   CSPFT_FLOAT        },
        {"maxTimeD3",     offsetof(DestructiblePiece, stages[3].maxTime),       CSPFT_FLOAT        },
        {"breakEffectD3", offsetof(DestructiblePiece, stages[3].breakEffect),   CSPFT_FX           },
        {"breakSoundD3",  offsetof(DestructiblePiece, stages[3].breakSound),    CSPFT_STRING       },
        {"breakNotifyD3", offsetof(DestructiblePiece, stages[3].breakNotify),   CSPFT_STRING       },
        {"loopSoundD3",   offsetof(DestructiblePiece, stages[3].loopSound),     CSPFT_STRING       },
        {"spawnModel1D3", offsetof(DestructiblePiece, stages[3].spawnModel[0]), CSPFT_XMODEL       },
        {"spawnModel2D3", offsetof(DestructiblePiece, stages[3].spawnModel[1]), CSPFT_XMODEL       },
        {"spawnModel3D3", offsetof(DestructiblePiece, stages[3].spawnModel[2]), CSPFT_XMODEL       },
        {"physPresetD3",  offsetof(DestructiblePiece, stages[3].physPreset),    CSPFT_PHYS_PRESET  },

        {"showBoneD4",    offsetof(DestructiblePiece, stages[4].showBone),      CSPFT_SCRIPT_STRING},
        {"breakHealthD4", offsetof(DestructiblePiece, stages[4].breakHealth),   CSPFT_FLOAT        },
        {"maxTimeD4",     offsetof(DestructiblePiece, stages[4].maxTime),       CSPFT_FLOAT        },
        {"breakEffectD4", offsetof(DestructiblePiece, stages[4].breakEffect),   CSPFT_FX           },
        {"breakSoundD4",  offsetof(DestructiblePiece, stages[4].breakSound),    CSPFT_STRING       },
        {"breakNotifyD4", offsetof(DestructiblePiece, stages[4].breakNotify),   CSPFT_STRING       },
        {"loopSoundD4",   offsetof(DestructiblePiece, stages[4].loopSound),     CSPFT_STRING       },
        {"spawnModel1D4", offsetof(DestructiblePiece, stages[4].spawnModel[0]), CSPFT_XMODEL       },
        {"spawnModel2D4", offsetof(DestructiblePiece, stages[4].spawnModel[1]), CSPFT_XMODEL       },
        {"spawnModel3D4", offsetof(DestructiblePiece, stages[4].spawnModel[2]), CSPFT_XMODEL       },
        {"physPresetD4",  offsetof(DestructiblePiece, stages[4].physPreset),    CSPFT_PHYS_PRESET  },
    };

    inline cspField_t destructible_def_fields[]{
        {"name",          offsetof(DestructibleDef, name),          CSPFT_STRING},
        {"model",         offsetof(DestructibleDef, model),         CSPFT_XMODEL},
        {"pristineModel", offsetof(DestructibleDef, pristineModel), CSPFT_XMODEL},
        {"numPieces",     offsetof(DestructibleDef, numPieces),     CSPFT_INT   },
        {"pieces",        offsetof(DestructibleDef, pieces),        CSPFT_INT   },
        {"clientOnly",    offsetof(DestructibleDef, clientOnly),    CSPFT_INT   },
    };
} // namespace T5
