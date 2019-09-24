#pragma once

#define LTC_NO_PROTOTYPES
#include "tomcrypt.h"

class CryptoLibrary
{
public:
    static void Init()
    {
        static bool initialized = false;

        if(!initialized)
        {
            initialized = true;

            ltc_mp = ltm_desc;
        }
    }
};