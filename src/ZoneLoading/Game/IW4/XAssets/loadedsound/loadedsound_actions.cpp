#include "loadedsound_actions.h"

#include <cstring>

using namespace IW4;

Actions_LoadedSound::Actions_LoadedSound(Zone& zone)
    : AssetLoadingActions(zone)
{
}

void Actions_LoadedSound::SetSoundData(MssSound* sound) const
{
    if (sound->info.data_len > 0)
    {
        const auto* tempData = sound->data;
        sound->data = m_zone.GetMemory()->Alloc<char>(sound->info.data_len);
        memcpy(sound->data, tempData, sound->info.data_len);
    }
    else
    {
        sound->data = nullptr;
    }
}
