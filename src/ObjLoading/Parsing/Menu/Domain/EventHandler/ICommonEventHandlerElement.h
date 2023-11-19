#pragma once

#include "Utils/ClassUtils.h"

namespace menu
{
    enum class CommonEventHandlerElementType
    {
        SCRIPT,
        CONDITION,
        SET_LOCAL_VAR
    };

    class ICommonEventHandlerElement
    {
    protected:
        ICommonEventHandlerElement() = default;

    public:
        virtual ~ICommonEventHandlerElement() = default;
        ICommonEventHandlerElement(const ICommonEventHandlerElement& other) = default;
        ICommonEventHandlerElement(ICommonEventHandlerElement&& other) noexcept = default;
        ICommonEventHandlerElement& operator=(const ICommonEventHandlerElement& other) = default;
        ICommonEventHandlerElement& operator=(ICommonEventHandlerElement&& other) noexcept = default;

        _NODISCARD virtual CommonEventHandlerElementType GetType() const = 0;
    };
} // namespace menu
