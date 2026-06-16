#pragma once

#include "FastFileContext.h"
#include "Utils/DispatchableThread.h"
#include "Web/WebWindowedLib.h"

#include <memory>

class ModManContext
{
public:
    static ModManContext& Get();

    void Startup();
    void Destroy();

    std::shared_ptr<webwindowed::window> m_main_window;
    std::shared_ptr<webwindowed::window> m_dev_tools_window;
    FastFileContext m_fast_file;

    DispatchableThread m_db_thread;
};
