#pragma once

#include "Parsing/IPackValueSupplier.h"
#include "Utils/ClassUtils.h"

class MockPackValueSupplier final : public IPackValueSupplier
{
public:
    _NODISCARD int GetCurrentPack() const override;
};
