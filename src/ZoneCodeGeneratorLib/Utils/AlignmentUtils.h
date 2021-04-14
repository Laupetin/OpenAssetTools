#pragma once

class AlignmentUtils
{
public:
    template<typename T>
    static T Align(T number, T alignmentValue)
    {
        if (alignmentValue == 0)
            return number;

        return (number + (alignmentValue - 1)) / alignmentValue * alignmentValue;
    }
};