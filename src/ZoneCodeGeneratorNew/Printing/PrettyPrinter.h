#pragma once
#include "Persistence/IDataRepository.h"

class PrettyPrinter
{
public:
    void Print(const IDataRepository* repository);
};
