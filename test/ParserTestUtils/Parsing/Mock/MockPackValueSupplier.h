#pragma once

#include "Utils/ClassUtils.h"
#include "Parsing/IPackValueSupplier.h"

class MockPackValueSupplier final : public IPackValueSupplier
{
public:
    _NODISCARD int GetCurrentPack() const override;
};
