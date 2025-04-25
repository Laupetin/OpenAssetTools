#pragma once

#include "IHashFunction.h"

#include <memory>

namespace cryptography
{
    std::unique_ptr<IHashFunction> CreateSha1();
}
