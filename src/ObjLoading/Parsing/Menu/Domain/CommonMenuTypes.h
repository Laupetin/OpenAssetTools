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
    };

    struct CommonRect
    {
        double x;
        double y;
        double w;
        double h;
        int horizontalAlign;
        int verticalAlign;
    };
}
