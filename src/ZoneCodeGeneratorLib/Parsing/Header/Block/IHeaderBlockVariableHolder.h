#pragma once

#include <memory>

#include "Domain/Definition/Variable.h"

class IHeaderBlockVariableHolder
{
public:
    IHeaderBlockVariableHolder() = default;
    virtual ~IHeaderBlockVariableHolder() = default;
    IHeaderBlockVariableHolder(const IHeaderBlockVariableHolder& other) = default;
    IHeaderBlockVariableHolder(IHeaderBlockVariableHolder&& other) noexcept = default;
    IHeaderBlockVariableHolder& operator=(const IHeaderBlockVariableHolder& other) = default;
    IHeaderBlockVariableHolder& operator=(IHeaderBlockVariableHolder&& other) noexcept = default;

    virtual void AddVariable(std::shared_ptr<Variable> variable) = 0;
};
