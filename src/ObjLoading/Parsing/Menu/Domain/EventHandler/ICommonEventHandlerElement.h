#pragma once

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

        virtual CommonEventHandlerElementType GetType() = 0;
    };
}