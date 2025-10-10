#pragma once

#include "Web/WebViewLib.h"

#include <memory>

class ModManContext
{
public:
    static ModManContext& Get();

    std::unique_ptr<webview::webview> m_main_webview;
    std::unique_ptr<webview::webview> m_dev_tools_webview;
};
