#include "CommonMenuTypes.h"

#include <cmath>
#include <limits>

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

bool CommonColor::Equals(const CommonColor& other) const
{
    return std::fabs(this->r - other.r) < std::numeric_limits<double>::epsilon() && std::fabs(this->g - other.g) < std::numeric_limits<double>::epsilon()
           && std::fabs(this->b - other.b) < std::numeric_limits<double>::epsilon() && std::fabs(this->a - other.a) < std::numeric_limits<double>::epsilon();
}

CommonRect::CommonRect()
    : CommonRect(0, 0, 0, 0)
{
}

CommonRect::CommonRect(const double x, const double y, const double w, const double h)
    : CommonRect(x, y, w, h, 0, 0)
{
}

CommonRect::CommonRect(const double x, const double y, const double w, const double h, const int horizontalAlign, const int verticalAlign)
    : x(x),
      y(y),
      w(w),
      h(h),
      horizontalAlign(horizontalAlign),
      verticalAlign(verticalAlign)
{
}
