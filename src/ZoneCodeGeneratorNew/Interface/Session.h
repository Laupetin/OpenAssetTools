#pragma once
#include "ZoneCodeGeneratorArguments.h"

class Session
{
    ZoneCodeGeneratorArguments m_args;

public:
    Session();
    explicit Session(ZoneCodeGeneratorArguments args);
};
