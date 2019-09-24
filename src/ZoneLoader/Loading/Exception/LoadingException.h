#pragma once
#include <exception>
#include <string>

class LoadingException : public std::exception
{
public:
    virtual std::string DetailedMessage() = 0;
};
