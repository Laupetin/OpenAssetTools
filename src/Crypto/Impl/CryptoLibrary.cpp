#include "CryptoLibrary.h"
#include "tommath.h"

void CryptoLibrary::Init()
{
    static bool initialized = false;

    if (!initialized)
    {
        initialized = true;

        ltc_mp = ltm_desc;
    }
}