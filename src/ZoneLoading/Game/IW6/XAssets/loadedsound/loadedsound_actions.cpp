#include "loadedsound_actions.h"

#include <cstring>

using namespace IW6;

Actions_LoadedSound::Actions_LoadedSound(Zone& zone)
    : AssetLoadingActions(zone)
{
}

void Actions_LoadedSound::SetSoundData(XA2Sound* sound) const
{
    
    if (sound->format.dataByteCount > 0)
    {
        unsigned char* tempData = sound->data;
        sound->data = m_zone.Memory().Alloc<unsigned char>(sound->loadedSize);
        memcpy(sound->data, tempData, sound->loadedSize);
    }
    else
    {
        sound->data = nullptr;
    }
}
