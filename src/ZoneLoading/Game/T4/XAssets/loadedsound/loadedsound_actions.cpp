#include "loadedsound_actions.h"

#include <cstring>

using namespace T4;

Actions_LoadedSound::Actions_LoadedSound(Zone& zone)
    : AssetLoadingActions(zone)
{
}

void Actions_LoadedSound::SetSoundData(snd_asset* sound) const
{
    if (sound->data_size > 0)
    {
        const auto* tempData = sound->data;
        sound->data = m_zone.Memory().Alloc<char>(sound->data_size);
        memcpy(sound->data, tempData, sound->data_size);
    }
    else
    {
        sound->data = nullptr;
    }
}
