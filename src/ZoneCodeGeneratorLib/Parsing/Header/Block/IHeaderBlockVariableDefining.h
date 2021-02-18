#pragma once

#include <string>

#include "Domain/Definition/DataDefinition.h"

class IHeaderBlockVariableDefining
{
public:
    IHeaderBlockVariableDefining() = default;
    virtual ~IHeaderBlockVariableDefining() = default;
    IHeaderBlockVariableDefining(const IHeaderBlockVariableDefining& other) = default;
    IHeaderBlockVariableDefining(IHeaderBlockVariableDefining&& other) noexcept = default;
    IHeaderBlockVariableDefining& operator=(const IHeaderBlockVariableDefining& other) = default;
    IHeaderBlockVariableDefining& operator=(IHeaderBlockVariableDefining&& other) noexcept = default;

    virtual bool IsDefiningVariable() = 0;
    virtual DataDefinition* GetVariableType() = 0;
    virtual std::string GetVariableName() = 0;
};
