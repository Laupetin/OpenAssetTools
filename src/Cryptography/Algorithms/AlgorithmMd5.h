#pragma once

#include "IHashFunction.h"

#include <memory>

namespace cryptography
{
    std::unique_ptr<IHashFunction> CreateMd5();
}
