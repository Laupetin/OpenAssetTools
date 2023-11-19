#pragma once

namespace menu
{
    union CommonColor
    {
        struct
        {
            double r;
            double g;
            double b;
            double a;
        };

        double array[4];

        CommonColor();
        CommonColor(double r, double g, double b, double a);

        bool Equals(const CommonColor& other) const;
    };

    struct CommonRect
    {
        double x;
        double y;
        double w;
        double h;
        int horizontalAlign;
        int verticalAlign;

        CommonRect();
        CommonRect(double x, double y, double w, double h);
        CommonRect(double x, double y, double w, double h, int horizontalAlign, int verticalAlign);
    };
} // namespace menu
