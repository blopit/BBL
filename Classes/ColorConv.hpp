//
//  ColorConv.hpp
//  BBL
//
//  Created by Shrenil Patel on 2018-06-05.
//

#ifndef ColorConv_hpp
#define ColorConv_hpp

#include <stdio.h>

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

hsv rgb2hsv(rgb in);
rgb hsv2rgb(hsv in);

#endif /* ColorConv_hpp */
