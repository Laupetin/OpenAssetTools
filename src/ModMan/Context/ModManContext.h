#pragma once

#include "FastFileContext.h"
#include "Utils/DispatchableThread.h"
#include "Web/WebViewLib.h"

#include <memory>

class ModManContext
{
public:
    static ModManContext& Get();

    void Startup();
    void Destroy();

    std::unique_ptr<webview::webview> m_main_webview;
    std::unique_ptr<webview::webview> m_dev_tools_webview;
    FastFileContext m_fast_file;

    DispatchableThread m_db_thread;
};
