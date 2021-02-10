#pragma once

class AlignmentUtils
{
public:
    template<typename T>
    static T Align(T number, T alignmentValue)
    {
        return (number + (alignmentValue - 1)) / alignmentValue * alignmentValue;
    }
};