#include "StepVerifyMagic.h"

#include <cstring>

#include "Loading/Exception/InvalidMagicException.h"

StepVerifyMagic::StepVerifyMagic(const char* magic)
{
    m_magic = magic;
    m_magic_len = strlen(m_magic);
}

void StepVerifyMagic::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    char currentCharacter;

    for(unsigned i = 0; i < m_magic_len; i++)
    {
        stream->Load(&currentCharacter, sizeof(char));

        if(currentCharacter != m_magic[i])
            throw InvalidMagicException(m_magic);
    }
}