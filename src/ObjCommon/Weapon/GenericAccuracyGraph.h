#pragma once
#include <string>
#include <vector>

class GenericAccuracyGraphKnot
{
public:
    float x;
    float y;
};

class GenericAccuracyGraph
{
public:
    std::string name;
    std::vector<GenericAccuracyGraphKnot> knots;
};
