#pragma once
#include <string>
#include <vector>

class GenericGraph2DKnot
{
public:
    double x;
    double y;

    GenericGraph2DKnot() = default;

    GenericGraph2DKnot(const double x, const double y)
        : x(x),
          y(y)
    {
    }
};

class GenericGraph2D
{
public:
    std::string name;
    std::vector<GenericGraph2DKnot> knots;
};
