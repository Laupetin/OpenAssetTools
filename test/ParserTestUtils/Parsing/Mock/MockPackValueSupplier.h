#pragma once

#include "Parsing/IPackValueSupplier.h"

class MockPackValueSupplier final : public IPackValueSupplier
{
public:
    [[nodiscard]] int GetCurrentPack() const override;
};
