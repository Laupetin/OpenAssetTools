#pragma once

#include "IStreamCipher.h"

#include <memory>

namespace cryptography
{
    std::unique_ptr<IStreamCipher> CreateSalsa20(const uint8_t* keyBytes, size_t keySize);
}
