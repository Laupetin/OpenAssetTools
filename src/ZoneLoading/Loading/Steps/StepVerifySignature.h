#pragma once

#include "Cryptography.h"
#include "Loading/ILoadingStep.h"
#include "Loading/ISignatureProvider.h"
#include "Utils/ICapturedDataProvider.h"

#include <memory>

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepVerifySignature(std::unique_ptr<cryptography::IPublicKeyAlgorithm> signatureAlgorithm,
                                                            ISignatureProvider* signatureProvider,
                                                            ICapturedDataProvider* signatureDataProvider);
}
