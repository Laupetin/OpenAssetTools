#include "ModManContext.h"

ModManContext& ModManContext::Get()
{
    static ModManContext context;
    return context;
}

void ModManContext::Startup()
{
    m_db_thread.Start();
}

void ModManContext::Destroy()
{
    m_db_thread.Terminate();
}
