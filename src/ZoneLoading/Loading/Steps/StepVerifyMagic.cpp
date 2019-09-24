#include "StepVerifyMagic.h"
#include "Loading/Exception/InvalidMagicException.h"

StepVerifyMagic::StepVerifyMagic(const char* magic)
{
    m_magic = magic;
}

void StepVerifyMagic::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    const size_t magicLength = strlen(m_magic);
    char currentCharacter;

    for(unsigned i = 0; i < magicLength; i++)
    {
        stream->Load(&currentCharacter, sizeof(char));

        if(currentCharacter != m_magic[i])
            throw InvalidMagicException(m_magic);
    }
}