#pragma once

#include <string>

#include "ICommonEventHandlerElement.h"

namespace menu
{
    class CommonEventHandlerScript final : public ICommonEventHandlerElement
    {
    public:
        std::string m_script;

        CommonEventHandlerScript();
        explicit CommonEventHandlerScript(std::string script);

        CommonEventHandlerElementType GetType() override;
    };
}
