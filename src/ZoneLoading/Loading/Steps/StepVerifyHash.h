#pragma once

#include "Cryptography.h"
#include "Loading/IHashProvider.h"
#include "Loading/ILoadingStep.h"
#include "Utils/ICapturedDataProvider.h"

#include <memory>

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepVerifyHash(std::unique_ptr<cryptography::IHashFunction> hashFunction,
                                                       unsigned hashIndex,
                                                       IHashProvider* hashProvider,
                                                       ICapturedDataProvider* dataProvider);
}
