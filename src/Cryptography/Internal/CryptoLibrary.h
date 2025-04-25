#pragma once

#define LTC_NO_PROTOTYPES
#include "tomcrypt.h"

namespace cryptography::internal
{
    class CryptoLibrary
    {
    public:
        static void Init();
    };
} // namespace cryptography::internal
