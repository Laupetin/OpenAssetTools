#pragma once

#include "ICommonEventHandlerElement.h"

#include <string>

namespace menu
{
    class CommonEventHandlerScript final : public ICommonEventHandlerElement
    {
    public:
        std::string m_script;

        CommonEventHandlerScript();
        explicit CommonEventHandlerScript(std::string script);

        _NODISCARD CommonEventHandlerElementType GetType() const override;
    };
} // namespace menu
