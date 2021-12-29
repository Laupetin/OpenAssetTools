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
