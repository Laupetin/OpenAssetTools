#pragma once

#include "Loading/IHashProvider.h"
#include "Loading/ILoadingStep.h"
#include "Utils/ICapturedDataProvider.h"

#include <memory>

namespace step
{
    class IStepLoadHash : public ILoadingStep, public IHashProvider, public ICapturedDataProvider
    {
    };

    std::unique_ptr<IStepLoadHash> CreateStepLoadHash(size_t hashSize, unsigned hashCount);
} // namespace step
