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
}
