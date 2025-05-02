#pragma once

#include "Loading/ILoadingStep.h"
#include "Loading/ISignatureProvider.h"

#include <memory>

namespace step
{
    class IStepLoadSignature : public ILoadingStep, public ISignatureProvider
    {
    };

    std::unique_ptr<IStepLoadSignature> CreateStepLoadSignature(size_t signatureSize);
} // namespace step
