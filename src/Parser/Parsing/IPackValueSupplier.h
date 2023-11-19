#pragma once

class IPackValueSupplier
{
public:
    IPackValueSupplier() = default;
    virtual ~IPackValueSupplier() = default;
    IPackValueSupplier(const IPackValueSupplier& other) = default;
    IPackValueSupplier(IPackValueSupplier&& other) noexcept = default;
    IPackValueSupplier& operator=(const IPackValueSupplier& other) = default;
    IPackValueSupplier& operator=(IPackValueSupplier&& other) noexcept = default;

    virtual int GetCurrentPack() const = 0;
};
