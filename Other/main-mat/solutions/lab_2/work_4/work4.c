#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>


bool isConvexPolygon(int n, ...) {
    double eps = 1e-5;
    va_list points;
    va_start(points, n);
    if (n < 3) { return false; }

    double x_, y_;
    double x1 = va_arg(points, double);
    double y1 = va_arg(points, double);

    x_ = x1; y_=y1;
    double x2, y2;
    double pref_square = 0;
    double first_no_zero = 0;
    for(int i = 1; i <= n; i++) 
    {

        x2 = va_arg(points, double);
        y2 = va_arg(points, double);
        double areaSign = x1 * y2 - x2 * y1;

        if (i == 1) pref_square = areaSign;
        if (first_no_zero == 0 && areaSign != 0 ) { first_no_zero = areaSign;}

        
        if (-eps <= areaSign && eps >= areaSign) {x1 = x2; y1 = y2; continue;}

        if (first_no_zero * areaSign < 0 && first_no_zero * (areaSign + eps) > 0) {areaSign += eps;}
        if (pref_square * areaSign < 0 && pref_square * (areaSign + eps) > 0) {areaSign += eps;}

        x1 = x2; y1 = y2;

        if((areaSign * pref_square < 0 || first_no_zero * areaSign < 0)) 
        {
            va_end(points);
            return false;
        }
    }
    double areaSign = x1 * y_ - x_ * y1;
    //printf("%lf\n", areaSign * pref_square );
    if(areaSign * pref_square + eps < 0) 
    {
        va_end(points);
        return false;
    }
    
    va_end(points);
    return true;
}

double polynomialValue(double x, int n, ...) {
    va_list coefficients;
    va_start(coefficients, n);

    double result = 0;
    for(int i = 0; i < n; i++) 
    {
        result *= x;

        double coeff = va_arg(coefficients, double);
        result += coeff;
    }

    va_end(coefficients);
    return result;
}
