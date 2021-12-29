#include "CommonMenuTypes.h"

using namespace menu;

CommonColor::CommonColor()
    : array{}
{
    r = 0.0;
    g = 0.0;
    b = 0.0;
    a = 0.0;
}

CommonColor::CommonColor(const double r, const double g, const double b, const double a)
    : array{}
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}
