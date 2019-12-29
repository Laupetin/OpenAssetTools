#pragma once

#include <string>

class IObjContainer
{
public:
    virtual ~IObjContainer() = default;

    virtual const std::string& GetName() = 0;
};