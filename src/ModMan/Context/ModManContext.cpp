#include "ModManContext.h"

ModManContext& ModManContext::Get()
{
    static ModManContext context;
    return context;
}
