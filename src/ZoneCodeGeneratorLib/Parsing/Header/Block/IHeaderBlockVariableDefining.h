#pragma once

#include <string>

class IHeaderBlockVariableDefining
{
public:
    IHeaderBlockVariableDefining() = default;
    virtual ~IHeaderBlockVariableDefining() = default;
    IHeaderBlockVariableDefining(const IHeaderBlockVariableDefining& other) = default;
    IHeaderBlockVariableDefining(IHeaderBlockVariableDefining&& other) noexcept = default;
    IHeaderBlockVariableDefining& operator=(const IHeaderBlockVariableDefining& other) = default;
    IHeaderBlockVariableDefining& operator=(IHeaderBlockVariableDefining&& other) noexcept = default;

    virtual void SetVariableName(std::string name) = 0;
};
