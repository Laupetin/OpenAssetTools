#pragma once

#include <string>

class IObjContainer
{
public:
    virtual ~IObjContainer() = default;

    virtual std::string GetName() = 0;
};
